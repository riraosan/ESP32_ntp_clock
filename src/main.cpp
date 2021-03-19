/*
The MIT License (MIT)

Copyright (c) 2020-2021 riraotech.com

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

#define HOSTNAME "atom_clock"
#define AP_NAME "ATOM-G-AP"

#define CLK 2
#define DIO 3

Ticker clocker;

TM1637Display display(CLK, DIO);

uint16_t timeLabelId;

void printLCD(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    char buffer[128] = {0};
    sprintf(buffer, "%02d%02d", tm->tm_hour, tm->tm_min);
    String _time(buffer);

    log_d("%d", _time.toInt());

    display.showNumberDec(_time.toInt(), true);
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
}

void initClock(void)
{
    //Get NTP Time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);

    clocker.attach_ms(1000, _clock);
}

void initESPUI(void)
{
    ESPUI.setVerbosity(Verbosity::Quiet);
    timeLabelId = ESPUI.addControl(ControlType::Label, "[ Date & Time ]", "0", ControlColor::Emerald, Control::noParent);
    ESPUI.begin("ESP32 NTP Clock");
}

void initLCD(void)
{
    uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
    //uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};
    display.setBrightness(0x0f);

    // All segments on
    display.setSegments(data);
    delay(2000);

    // Selectively set different digits
    data[0] = display.encodeDigit(0);
    data[1] = display.encodeDigit(1);
    data[2] = display.encodeDigit(2);
    data[3] = display.encodeDigit(3);
    display.setSegments(data);
    delay(2000);
}

void setup(void)
{
    STB.setHostname(HOSTNAME);
    STB.setApName(AP_NAME);
    STB.begin(false, false);

    initClock();
    initESPUI();
    initLCD();
}

void loop(void)
{
    STB.handle();

    yield();
}
