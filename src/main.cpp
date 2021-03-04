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

#define HOSTNAME "esp32_clock"
#define AP_NAME "ESP32-G-AP"

Ticker clocker;
SerialTelnetBridgeClass STB;

uint16_t timeLabelId;

String getTime()
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

    log_n("[date & time] %s", String(buffer).c_str());

    return String(buffer);
}

void _clock()
{
    ESPUI.updateControlValue(timeLabelId, getTime());
}

void initClock()
{
    //Get NTP Time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);

    clocker.attach_ms(1000, _clock);
}

void initESPUI()
{
    ESPUI.setVerbosity(Verbosity::VerboseJSON);
    timeLabelId = ESPUI.addControl(ControlType::Label, "[ Date & Time ]", "0", ControlColor::Emerald, Control::noParent);
    ESPUI.begin("ESP32 NTP Clock");
}

void setup()
{
    STB.setHostname(HOSTNAME);
    STB.setApName(AP_NAME);
    STB.begin(false, false, false);

    initClock();
    initESPUI();
}

void loop()
{
    STB.handle();



    yield();
}
