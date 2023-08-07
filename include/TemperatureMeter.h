// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Configuration.h"
#include <espMqttClient.h>
#include <Arduino.h>
#include <Hoymiles.h>
#include <memory>

#ifndef OW_PIN
#define OW_PIN 15
#endif

class TemperatureMeterClass {
public:
    void init();
    void mqtt();
    void loop();
    float TempVal = 0.0;

private:
    uint32_t _lastTemperatureMeterCheck;
    void readTemperatureMeter();
};

extern TemperatureMeterClass TemperatureMeter;
