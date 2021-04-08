
# ⏰ NTP Clock example for ESP32 (M5Stack ATOM Lite)

## 🍀 Overview

This program is an NTP clock that runs on [M5Stack ATOM Lite](https://m5stack-store.myshopify.com/products/atom-lite-esp32-development-kit).
It is possible to display the time, temperature, humidity, and barometric pressure by controlling the 4-digit,7-segment LED of [LED digital clock "Holtz (M)"](https://www.nitori-net.jp/ec/product/8171416/) released by the Japanese home center [NITORI](https://www.nitori.co.jp/en/service/estore/).
The 7-segment LED driver IC uses [TM1637](https://github.com/avishorp/TM1637).
[BME280](https://github.com/adafruit/Adafruit_BME280_Library) is used to measure temperature, humidity and barometric pressure.

## 📷 Photos

<img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-03-21/IMG_1384.png?raw=true" width="300"/><img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-03-21/IMG_1382.png?raw=true" width="300"/><img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1387.png?raw=true" width="300"/>

## 🏗️ How to use

1. Build the source code with PlatformIO and write it to ATOM Lite.
2. Connect this NTP clock to your Home WiFi access point.
   <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1503.png?raw=true" width="200"/><img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1505.png?raw=true" width="200"/>

3. Access "http://atom_clcok.local" from the browser of a PC or smartphone connected to the same Home WiFi access point.
4. The temperature, humidity, barometric pressure, and time will be displayed in the GUI created by ESPUI.
   <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1491.png?raw=true" width="200"/><img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1492.png?raw=true" width="200"/>

## 🤖 Functions

- [x] The 1st digit dot of the 7-segment LED blinks 0.3 second cycle while the access point is connecting.
- [x] the 1st digit 7-segment LED displays "0000" while the access point is connecting.
- [x] Temperature, humidity and barometric pressure are displayed for  evry 60 seconds.
- [x] LED display pattern: Time(5 sec) -> Temperature(2 sec) -> Humidity(2 sec) -> Pressure(2 sec) -> Display off for about 48 seconds
- [x] The time is displayed in "HH:MM" format.
- [x] If you press the button(G39), the connection to the previously connected access point will be canceled, and you will be able to set the connection to the new access point.

## 🍞 Breadboard diagram (by fritzing)

<img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-04/ESP32_ATOM_Lite_TM1637_NTP_Clock_%E3%83%96%E3%83%AC%E3%83%83%E3%83%89%E3%83%9C%E3%83%BC%E3%83%89.png?raw=true" width="400"/>

## 👤 Author

- GitHub: [riraosan](https://github.com/riraosan)
- Qiita: [riaosan](https://qiita.com/riraosan)

## 📝 License

This software is released under the [MIT](https://github.com/riraosan/ESP32_ntp_clock/blob/main/LICENSE) License.

## ☕ Contribute

Liked this Code? You can support me by sending me a [☕ Coffee](https://paypal.me/riraosan?locale.x=ja_JP).

Otherwise I really welcome Pull Requests.

---

Translated by [Google Translate](https://translate.google.co.jp/).
