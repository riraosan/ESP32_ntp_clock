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
#include <esp32-hal-log.h>
#include <TM1637Display.h>
#include <BME280Class.h>

#define HOSTNAME "atom_clock"
#define AP_NAME "ATOM-G-AP"

#define CLK 22
#define DIO 19

Ticker clocker;

TM1637Display display(CLK, DIO);

uint16_t timeLabelId;

void printLCD(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    char buffer[16] = {0};
    sprintf(buffer, "%02d%02d", tm->tm_hour, tm->tm_min);
    String _time(buffer);

    static uint8_t flag = 0;
    flag = ~flag;

    if (flag)
    {
        display.showNumberDec(_time.toInt(), false);
    }
    else
    {
        display.showNumberDecEx(_time.toInt(), (0x80 >> 2), false);
    }
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
    printLCD();
}

void initClock(void)
{
    //Get NTP Time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);

    clocker.attach_ms(500, _clock);
}

void initESPUI(void)
{
    ESPUI.setVerbosity(Verbosity::Quiet);
    timeLabelId = ESPUI.addControl(ControlType::Label, "[ Date & Time ]", "0", ControlColor::Emerald, Control::noParent);
    ESPUI.begin("ESP32 NTP Clock");
}

void displayOn(void)
{
    display.setBrightness(7, true);
    display.showNumberDec(0, true);
}

void displayOff(void)
{
    display.setBrightness(7, false);
    display.showNumberDec(0, false);
}

void setup(void)
{
    displayOff();

    STB.setHostname(HOSTNAME);
    STB.setApName(AP_NAME);
    STB.begin(false, false);

    initClock();
    initESPUI();
    bme280.setup(25, 21);

    displayOn();
}

void loop(void)
{
    STB.handle();

    yield();
}
