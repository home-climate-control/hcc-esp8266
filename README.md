# Home Climate Control ESP8266 based edge device firmware

## Short list
* [WiFi enabled MQTT sender for 1-Wire temperature sensors](https://github.com/home-climate-control/hcc-ESP8266/tree/master/ES8266-mqtt-wifi-onewire)

## Development hardware

* The board: [Adafruit Feather HUZZAH with ESP8266](https://www.adafruit.com/product/3213), reason being - it features 1S LiPo battery connector and charger. Given the fact that the devices being developed will be most likely remote and wireless, this is a serious advantage. YMMV.
* Temperature sensors: [Maxim DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf). Cheap as dirt, digital, noise free, and reliable.
