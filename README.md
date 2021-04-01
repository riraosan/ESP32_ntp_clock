
# ESP32 NTP Clock

## NTP Clock example for ESP32 (M5Stack ATOM Lite)

This program is an NTP clock that runs on [M5Stack ATOM Lite](https://m5stack-store.myshopify.com/products/atom-lite-esp32-development-kit).
It is possible to display the time, temperature, humidity, and barometric pressure by controlling the 4-digit,7-segment LED of [LED digital clock "Holtz (M)"](https://www.nitori-net.jp/ec/product/8171416/) released by the Japanese home center [NITORI](https://www.nitori.co.jp/en/service/estore/).
The 7-segment LED driver IC uses [TM1637](https://github.com/avishorp/TM1637).
[BME280](https://github.com/adafruit/Adafruit_BME280_Library) is used to measure temperature, humidity and barometric pressure.

## Functions (updating)

- [x] The 1st digit dot of the 7-segment LED blinks 0.3 second cycle while the access point is connecting.
- [x] the 1st digit 7-segment LED displays "8" while the access point is connecting.
- [x] Temperature, humidity and barometric pressure are displayed every minute for 2 seconds.
- [x] LED display pattern: Time(5 sec) -> Temperature(2 sec) -> Humidity(2 sec) -> Pressure(2 sec) -> Display off for about 48 seconds
- [x] The time is displayed in "HH:MM" format.
- [ ] If you press the button, the connection to the previously connected access point will be canceled, and you will be able to set the connection to the new access point.

---

Powered by Google Translate ;-)
