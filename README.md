# Home Climate Control ESP8266 based edge device firmware

## Short list
* [WiFi enabled MQTT sender for 1-Wire temperature sensors](https://github.com/home-climate-control/hcc-ESP8266/tree/master/ES8266-mqtt-wifi-onewire)

## Development hardware

* The board: [Adafruit Feather HUZZAH with ESP8266](https://www.adafruit.com/product/3213), reason being - it features 1S LiPo battery connector and charger. Given the fact that the devices being developed will be most likely remote and wireless, this is a serious advantage. YMMV.
* Temperature sensors: [Maxim DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf). Cheap as dirt, digital, noise free, and reliable.

## Pending integrations

The following hardware is already on the bench, waiting to be provided support for:

* [AM2320 Digital Temperature and Humidity Sensor](https://www.adafruit.com/product/3721)
* [MCP9808 High Accuracy I2C Temperature Sensor](https://www.adafruit.com/product/1782)

## Won't do

* [DHT11](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf). According to lore collected during [Home Climate Control](https://github.com/home-climate-control/dz/wiki) development, it's not good enough.

# What's next?

You might want to explore the [Home Climate Control](https://github.com/home-climate-control/dz) project, for which these edge devices were intended in the first place.
