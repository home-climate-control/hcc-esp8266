# Home Climate Control ESP8266 based edge device firmware

## Status

Suspended until further notice, reason being: [ESPHome](http://esphome.io/) provides overwhelmingly more functionality for ESP8266 platform, and is easily integrated (see [DZ to ESPHome Integration](https://github.com/home-climate-control/dz/wiki/HOWTO:-DZ-to-ESPHome-integration)).

It's possible that it will be reanimated at some point, there a few things ESPHome cannot do with required by DZ level of finesse.

## Development hardware

* The board: [Adafruit Feather HUZZAH with ESP8266](https://www.adafruit.com/product/3213), reason being - it features 1S LiPo battery connector and charger. Given the fact that the devices being developed will be most likely remote and wireless, this is a serious advantage. YMMV.
* Temperature sensors: [Maxim DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf). Cheap as dirt, digital, noise free, and reliable.

# What's next?

* You might want to explore the [Home Climate Control](https://github.com/home-climate-control/dz) project, for which these edge devices were intended in the first place.
* [hcc-esp32](https://github.com/home-climate-control/hcc-esp32) - basically, `hcc-esp8266++` on [ESP32](https://en.wikipedia.org/wiki/ESP32) hardware. This project **is** on the long term roadmap and will not get axed anytime soon, it will provide functionality ESPHome doesn't.
