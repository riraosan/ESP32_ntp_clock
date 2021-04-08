
# ⏰ NTP Clock example for ESP32 (M5Stack ATOM Lite)

## 🍀 Overview

This program is an NTP clock that runs on [M5Stack ATOM Lite](https://m5stack-store.myshopify.com/products/atom-lite-esp32-development-kit).
It is possible to display the time, temperature, humidity, and barometric pressure by controlling the 4-digit,7-segment LED of [LED digital clock "Holtz (M)"](https://www.nitori-net.jp/ec/product/8171416/) released by the Japanese home center [NITORI](https://www.nitori.co.jp/en/service/estore/).
The 7-segment LED driver IC uses [TM1637](https://github.com/avishorp/TM1637).
[BME280](https://github.com/adafruit/Adafruit_BME280_Library) is used to measure temperature, humidity and barometric pressure.

## 📷 Photos

<img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-03-21/IMG_1384.png?raw=true" width="300"/> <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-03-21/IMG_1382.png?raw=true" width="300"/> <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1387.png?raw=true" width="300"/>

## Dependencies

This library is dependent on the following libraries to function properly.

- https://github.com/alanswx/ESPAsyncWiFiManager.git
- https://github.com/me-no-dev/ESPAsyncWebServer.git
- https://github.com/lumostor/bitlash-esp32.git
- https://github.com/lumostor/arduino-esp32-Console.git
- https://github.com/lumostor/arduino-esp32-LinenoiseBitlash.git
- https://github.com/riraosan/ESP32SerialTelnetBridge.git
- https://github.com/s00500/ESPUI.git
- https://github.com/avishorp/TM1637.git
- https://github.com/adafruit/Adafruit_Sensor.git
- https://github.com/adafruit/Adafruit_BME280_Library.git
- https://github.com/LennartHennigs/Button2.git

## 🏗️ How to use

1. Build the source code with PlatformIO and write it to ATOM Lite.
2. Connect this NTP clock to your Home WiFi access point.

   <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1503.png?raw=true" width="200" align=top />
   <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1505.png?raw=true" width="200" align=top />

4. Access "http://atom_clcok.local" from the browser of a PC or smartphone connected to the same Home WiFi access point.
5. The temperature, humidity, barometric pressure, and time will be displayed in the GUI created by ESPUI.

   <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1491.png?raw=true" width="200"/>
   <img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-07/IMG_1492.png?raw=true" width="200"/>

## 🤖 Functions

- [x] The 1st digit dot of the 7-segment LED blinks 0.3 second cycle while the access point is connecting.
- [x] the 1st digit 7-segment LED displays "0000" while the access point is connecting.
- [x] Temperature, humidity and barometric pressure are displayed for every 60 seconds.
- [x] LED display pattern: Time(5 sec) -> Temperature(2 sec) -> Humidity(2 sec) -> Pressure(2 sec) -> Display off for about 48 seconds
- [x] The time is displayed in "HH:MM" format.
- [x] If you press the button(G39), the connection to the previously connected access point will be canceled, and you will be able to set the connection to the new access point.

## 🍞 Breadboard diagram (by fritzing)

<img src="https://github.com/riraosan/riraosan.github.io/blob/master/2021-04-04/ESP32_ATOM_Lite_TM1637_NTP_Clock_%E3%83%96%E3%83%AC%E3%83%83%E3%83%89%E3%83%9C%E3%83%BC%E3%83%89.png?raw=true" width="400"/>

## 💗 Acknowledgments

Thanks to the authors of these libraries.🤝

- Thanks to [alanswx](https://github.com/alanswx), author of the [ESPAsyncWiFiManager](https://github.com/alanswx/ESPAsyncWiFiManager.git) library.
- Thanks to [me-no-dev](https://github.com/me-no-dev), author of the [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer.git) library.
- Thanks to [lumostor](https://github.com/lumostor), author of the [bitlash-esp32](https://github.com/lumostor/bitlash-esp32.git) library, [arduino-esp32-Console](https://github.com/lumostor/arduino-esp32-Console.git) library, and [arduino-esp32-LinenoiseBitlash](https://github.com/lumostor/arduino-esp32-LinenoiseBitlash.git) library
- Thanks to [s00500](https://github.com/s00500), author of the [ESPUI](https://github.com/s00500/ESPUI.git) library.
- Thanks to [avishorp](https://github.com/avishorp), author of the [TM1637](https://github.com/avishorp/TM1637.git) library.
- Thanks to [Adafruit Industries](https://github.com/adafruit), author of the [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor.git) library, and [Adafruit_BME280_Library](https://github.com/adafruit/Adafruit_BME280_Library.git) library.
- Thanks to [LennartHennigs](https://github.com/LennartHennigs), author of the [Button2](https://github.com/LennartHennigs/Button2.git) library.

## 👤 Author

- GitHub: [riraosan](https://github.com/riraosan)
- Qiita: [riaosan](https://qiita.com/riraosan)

> Serenity Prayer
>
> God, grant me the serenity to accept the things I cannot change
>
> courage to change the things I can,
>
> and wisdom to know the difference.

[Reinhold Niebuhr](https://en.wikipedia.org/wiki/Reinhold_Niebuhr)

## 📝 License

This software is released under the [MIT](https://github.com/riraosan/ESP32_ntp_clock/blob/main/LICENSE) License.

## ☕ Contribute

Liked this Code? You can support me by sending me a [☕ Coffee](https://paypal.me/riraosan?locale.x=ja_JP).

Otherwise I really welcome Pull Requests.

---

Translated by [Google Translate](https://translate.google.co.jp/).
