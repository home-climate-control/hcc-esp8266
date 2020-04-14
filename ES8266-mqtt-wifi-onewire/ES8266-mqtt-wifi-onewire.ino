/*
 Basic ESP8266 MQTT temperature sensor.

 This sketch
 - Discovers 1-Wire devices at boot,
 - Connects to the specified WiFi,
 - Connects to the specified MQTT broker,
 - and then keeps sending temperature samples every specified number of milliseconds.

 It was created as a quick and dirty sample for DIY Zoning Project MQTT integration,
 (see https://github.com/home-climate-control/dz), but should work with other
 MQTT aware home automation suites.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

 VT: FIXME: Provide instructions for including other dependencies (PubSubClient, OneWire, DallasTemperature).
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*
 * secrets.h (which shouldn't be version controlled) should contain the following:
 *
 * const char* ssid = "wifi ssid";
 * const char* password = "wifi password";
 * const char* mqtt_server = "mqtt server hostname";
 *
 * const char* mqtt_root = "mqtt root topic"; // suggested value '/hcc'
 *
 * const long onewire_poll_interval_millis = POLL_INTERVAL; // suggested value 5000
 */
#include "secrets.h"

String getDeviceId()
{
    String result = "ESP8266-";
    char buf[8];
    sprintf(buf, "%08X", ESP.getChipId());
    String id = String(buf);
    return result + id;
}

String deviceId = getDeviceId();

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

// Data wire is plugged TO GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Number of temperature devices found
int numberOfDevices;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress;

void setup_wifi()
{

    delay(10);

    Serial.printf("WiFi: connecting to %s... ", ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.print("\nWiFi: IP ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{

    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.printf("MQTT: connecting to %s...\n", mqtt_server);

        if (client.connect(deviceId.c_str())) {
            Serial.printf("MQTT: connected as %s\n", deviceId.c_str());
            // Once connected, publish an announcement
            mqttHello();
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup_onewire()
{

    // Start up the library
    sensors.begin();

    // Grab a count of devices on the wire
    numberOfDevices = sensors.getDeviceCount();

    // locate devices on the bus
    Serial.printf("1-Wire: found %d devices\n", numberOfDevices);

    // Loop through each device, print out address
    for (int i = 0; i < numberOfDevices; i++) {
        // Search the wire for address
        if (sensors.getAddress(tempDeviceAddress, i)) {
            Serial.printf("1-Wire: %d: %s\n", i, getAddress(tempDeviceAddress).c_str());
        } else {
            Serial.print("Found ghost device at ");
            Serial.print(i, DEC);
            Serial.print(" but could not detect address. Check power and cabling");
        }
    }
}

void setup()
{
    // Initialize the BUILTIN_LED pin as an output - this will actually turn the LED on
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(115200);
    Serial.printf("\ndeviceId: %s\n", deviceId.c_str());
    setup_onewire();
    setup_wifi();
    client.setServer(mqtt_server, 1883);
}

void loop_onewire()
{
    // Turn on the LED to indicate 1-Wire operation
    digitalWrite(BUILTIN_LED, LOW);
    sensors.requestTemperatures();
    // Turn off the LED
    digitalWrite(BUILTIN_LED, HIGH);

    // Loop through each device, print out temperature data
    for (int i = 0; i < numberOfDevices; i++) {
        // Search the wire for address
        if (sensors.getAddress(tempDeviceAddress, i)) {
            String address = getAddress(tempDeviceAddress);
            float tempC = sensors.getTempC(tempDeviceAddress);

            // MQTT message will go to ${mqtt_root}/sensor/${address} topic
            // Payload will be formed like this:
            // For now:
            // {"entity_type":"sensor","name":"${address}","signature":"T${address}","signal":${double-value},"device_id":"${deviceId}"}
            // When NTP is integrated:
            // {"entity_type":"sensor","timestamp":${millis},"name":"${address}","signature":"T${address}","signal":${double-value},"device_id":"${deviceId}"}

            String payload = "{\"entity_type\":\"sensor\",\"name\":\"";
            payload += address;
            payload += "\",\"signature\":\"T";
            payload += address;
            payload += "\",\"signal\":";
            payload += tempC;
            payload += ",\"device_id\":\"";
            payload += deviceId;
            payload += "\"}";

            publishSamples(address, payload);
        }
    }
}


void publishSamples(String address, String payload)
{

    String topic = String(mqtt_root);
    topic += "/sensor/";
    topic += address;

    publish(topic, payload);
}

void publish(String topic, String payload)
{

    Serial.printf("%s: %s\n", topic.c_str(), payload.c_str());

    // Our payload may be too too long for a single publish() call, need to split
    int messageSize = payload.length();
    int leftToSend = messageSize;
    int offset = 0;

    // VT: NOTE: we don't want to set retain = true until the timestamp can be provided -
    // otherwise consumers might get quite confused
    client.beginPublish(topic.c_str(), messageSize, false);

    while (leftToSend > 0) {

        int packetSize = MQTT_MAX_PACKET_SIZE < leftToSend ? MQTT_MAX_PACKET_SIZE : leftToSend;
        leftToSend -= packetSize;
        String packet = payload.substring(offset, offset + packetSize);

        //Serial.printf("## size, packet/left %d, %d/%d\n", messageSize, packetSize, leftToSend);
        //Serial.printf("## packet: %s\n", packet.c_str());

        client.write((const uint8_t *)packet.c_str(), packetSize);

        offset += packetSize;
    }

    client.endPublish();
}

void loop()
{

    if (!client.connected()) {
        reconnect();
    }

    client.loop();

    unsigned long now = millis();
    if (now - lastMsg > onewire_poll_interval_millis) {
        lastMsg = now;

        loop_onewire();
    }
}

String getAddress(DeviceAddress deviceAddress)
{
    String result;
    for (uint8_t i = 0; i < 8; i++) {
        char buf[2];
        sprintf(buf, "%02X", deviceAddress[i]);
        result += String(buf);
    }
    return result;
}

void mqttHello()
{
    String topic = String(mqtt_root);
    topic += "/edge";

    // Hello message format is:
    // {"entityType":"sensor","deviceId":"${deviceId}","sources": ["${1-Wire-A}", ... "${1-Wire-N}"]}
    String payload = "{\"entityType\":\"sensor\",\"deviceId\":\"";
    payload += deviceId;
    payload += "\",\"sources\": [";

    for (int i = 0; i < numberOfDevices; i++) {

        if (sensors.getAddress(tempDeviceAddress, i)) {

            payload += "\"";
            payload += getAddress(tempDeviceAddress);
            payload += "\"";

            if (i < numberOfDevices - 1) {
                payload += ",";
            }
        }
    }

    payload += "]}";

    publish(topic, payload);
}
