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

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class BME280Class
{
public:
    BME280Class();
    ~BME280Class();

    static void sendSensorInfo(void);

    void initBME280HumiditySensing(void);
    void initBME280WeatherStation(void);
    void initUnifiedBME280(void);

    float getTemperature(void);
    float getPressure(void);
    float getHumidity(void);
    float getAltitude(float seaLevel);
    uint32_t getSensorID(void);

    void setup(int sdaPin, int sclPin);
    void handle(void);

private:

    Adafruit_BME280 *_bme;
    Adafruit_Sensor *_pressur;
    Adafruit_Sensor *_temperature;
    Adafruit_Sensor *_humidity;
    uint32_t _sensor_ID;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_BME280CLASS)
extern BME280Class bme280;
#endif
