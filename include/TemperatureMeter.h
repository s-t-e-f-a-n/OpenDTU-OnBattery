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
    //void mqtt();
    void loop();
    void readTemperatureMeter();

    float TempVal = 0.0;

    //void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
    //float getPowerTotal(bool forceUpdate = true);
    //uint32_t getLastPowerMeterUpdate();

private:
    //uint32_t _interval;
    uint32_t _lastTemperatureMeterCheck;
    // Used in Power limiter for safety check
    //uint32_t _lastPowerMeterUpdate;

    //float _powerMeter1Power = 0.0;
    //float _powerMeter2Power = 0.0;
    //float _powerMeter3Power = 0.0;
    //float _powerMeter1Voltage = 0.0;
    //float _powerMeter2Voltage = 0.0;
    //float _powerMeter3Voltage = 0.0;
    //float _powerMeterImport = 0.0;
    //float _powerMeterExport = 0.0;

    //bool mqttInitDone = false;

};

extern TemperatureMeterClass TemperatureMeter;
