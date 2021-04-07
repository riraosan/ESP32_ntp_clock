/*
The MIT License (MIT)

Copyright (c) 2020-2021 riraosan.github.io

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Arduino.h>
#include <SerialTelnetBridge.h>
#include <ESPUI.h>
#include <Ticker.h>
#include <TM1637Display.h>
#include <BME280Class.h>
#include <Button2.h>

#include <esp32-hal-log.h>

#define HOSTNAME "atom_clock"
#define AP_NAME "ATOM-G-AP"

#define TIME_ZONE "JST-9"
#define NTP_SERVER1 "ntp.nict.jp"
#define NTP_SERVER2 "ntp.jst.mfeed.ad.jp"
#define NTP_SERVER3 ""

#define CLK 22
#define DIO 19

#define SDA 25
#define SCL 21

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 48          /* Time ESP32 will go to sleep (in seconds) */

#define BUTTON_PIN 39

Button2 button = Button2(BUTTON_PIN);

Ticker clocker;
Ticker sensorChecker;
Ticker tempeChecker;
Ticker humidChecker;
Ticker pressChecker;
Ticker displaySwitcher;
Ticker sleeper;

TM1637Display display(CLK, DIO);

uint16_t timeLabelId;
uint16_t temperatureLabelId;
uint16_t humidityLabelId;
uint16_t pressureLabelId;

bool showSensor = false;

void _checkSensor(void)
{
    showSensor = true;
}

void printTemperature(float temp)
{
    char buffer[16] = {0};
    //LED
    sprintf(buffer, "0x%2.0fC", temp);

    display.clear();
    display.showNumberHexEx(strtol(buffer, 0, 16), (0x80 >> 2), false, 3, 1);

    //WebUI
    sprintf(buffer, "%2.1f℃", temp);
    String tempUI(buffer);
    ESPUI.updateControlValue(temperatureLabelId, tempUI);
}

void printHumidity(float temp)
{
    char buffer[16] = {0};
    //LED
    sprintf(buffer, "%2f", temp);
    String humidLed(buffer);
    display.clear();
    display.showNumberDecEx(humidLed.toInt(), (0x80 >> 0), false);

    //WebUI
    sprintf(buffer, "%2.1f%%", temp);
    String humidUI(buffer);
    ESPUI.updateControlValue(humidityLabelId, humidUI);
}

void printPressure(float temp)
{
    char buffer[16] = {0};
    //LED
    sprintf(buffer, "%4f", temp);
    String pressLed(buffer);
    display.clear();
    display.showNumberDecEx(pressLed.toInt(), (0x80 >> 0), false);

    //WebUI
    sprintf(buffer, "%2.1fhPa", temp);
    String pressUI(buffer);
    ESPUI.updateControlValue(pressureLabelId, pressUI);
}

void _checkTempe(void)
{
    clocker.detach();
    printTemperature(bme280.getTemperature());
}

void _checkHumid(void)
{
    printHumidity(bme280.getHumidity());
}

void _checkPress(void)
{
    printPressure(bme280.getPressure());
}

void printTime(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    char buffer[16] = {0};
    sprintf(buffer, "%02d%02d", tm->tm_hour, tm->tm_min);
    String _time(buffer);

    static uint8_t flag = 0;
    flag = ~flag;

    if (flag)
        display.showNumberDecEx(_time.toInt(), (0x80 >> 2), true);
    else
        display.showNumberDecEx(_time.toInt(), (0x80 >> 4), true);
}

String getTime(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    char buffer[128] = {0};
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d+0900",
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec);

    return String(buffer);
}

void _clock(void)
{
    ESPUI.updateControlValue(timeLabelId, getTime());
    printTime();
}

void initClock(void)
{
    configTzTime(TIME_ZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
}

void initESPUI(void)
{
    ESPUI.setVerbosity(Verbosity::Quiet);

    uint16_t tab1 = ESPUI.addControl(ControlType::Tab, "Date & Time", "Date & Time");
    uint16_t tab2 = ESPUI.addControl(ControlType::Tab, "Weather Station", "Weather Station");

    timeLabelId = ESPUI.addControl(ControlType::Label, "[ Date & Time ]", "0", ControlColor::Emerald, tab1);
    temperatureLabelId = ESPUI.addControl(ControlType::Label, "[ Temperature ]", "0", ControlColor::Sunflower, tab2);
    humidityLabelId = ESPUI.addControl(ControlType::Label, "[ Humidity ]", "0", ControlColor::Sunflower, tab2);
    pressureLabelId = ESPUI.addControl(ControlType::Label, "[ Pressure ]", "0", ControlColor::Sunflower, tab2);

    ESPUI.begin("ESP32 NTP Clock");
}

void displayOn(void)
{
    display.setBrightness(7, true);
    display.clear();
}

void displayOff(void)
{
    display.setBrightness(7, false);
    display.clear();
}

void initBME280(void)
{
    bme280.setup(SDA, SCL);
    sensorChecker.attach(60, _checkSensor);
}

void initLightSleep(void)
{
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    log_d("Setup ESP32 to sleep for every %s Seconds", String(TIME_TO_SLEEP).c_str());
}

void connecting(void)
{
    static uint8_t flag = 0;
    flag = ~flag;

    if (flag)
        display.showNumberDecEx(8, (0x80 >> 3), false);
    else
        display.showNumberDecEx(8, (0x80 >> 4), false);
}

void released(Button2 &btn)
{
    WiFi.disconnect(true, true);
    ESP.restart();
}

void initButton(void)
{
    button.setReleasedHandler(released);
}

void setup(void)
{
    displayOn();

    STB.setWiFiConnectChecker(connecting);
    STB.setHostname(HOSTNAME);
    STB.setApName(AP_NAME);
    STB.begin(false, false);

    displayOff();
    initClock();
    initESPUI();
    initBME280();
    initButton();

    showSensor = true;
}

void loop(void)
{
    STB.handle();
    button.loop();

    if (showSensor)
    {
        displayOn();
        clocker.detach();

        printTemperature(bme280.getTemperature());
        delay(2 * 1000);
        printHumidity(bme280.getHumidity());
        delay(2 * 1000);
        printPressure(bme280.getPressure());
        delay(2 * 1000);

        clocker.attach_ms(500, _clock);

        showSensor = false;
    }

    yield();
}
