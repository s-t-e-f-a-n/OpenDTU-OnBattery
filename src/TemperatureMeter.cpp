// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 s-c-h-m-i-t-t
 */


#include "TemperatureMeter.h"
#include "Configuration.h"
#include "HttpPowerMeter.h"
#include "MqttSettings.h"
#include "NetworkSettings.h"
#include "MessageOutput.h"
#include <ctime>
#include <SoftwareSerial.h>
#include "OneWire.h"
#include "DallasTemperature.h"

TemperatureMeterClass TemperatureMeter;

OneWire  ds(OW_PIN);
DallasTemperature sensors(&ds);

void TemperatureMeterClass::init()
{
    _lastTemperatureMeterCheck = 0;
    
    CONFIG_T& config = Configuration.get();
    
    if (!config.Temperature_Enabled) {
        return;
    }

    MessageOutput.print("Initialize Temperature Sensor... ");
    sensors.begin();
    if ( sensors.getDeviceCount() == 0 ) {
      config.Temperature_Enabled = false;
      Configuration.write();
      MessageOutput.print("No Temperature Sensor found: Disabled Confguration...");
    }
}

void TemperatureMeterClass::loop(void)
{
    CONFIG_T& config = Configuration.get();
    
    if (!config.Temperature_Enabled) {
        return;
    }

  if (millis() - _lastTemperatureMeterCheck > 1000) {

      readTemperatureMeter();

      _lastTemperatureMeterCheck = millis();

      mqtt();
  }
}

void TemperatureMeterClass::readTemperatureMeter(void)
{
  sensors.requestTemperatures(); // Send the command to get temperatures

  TempVal = sensors.getTempCByIndex(0);

  if (TempVal == DEVICE_DISCONNECTED_C) { MessageOutput.println("Error: Could not read temperature data"); }
}

void TemperatureMeterClass::mqtt()
{
    if (!MqttSettings.getConnected()) {
        return;
    } else {
        String topic = "temperaturemeter";
        MqttSettings.publish(topic + "/temperature1", String(TempVal));
    }
}



