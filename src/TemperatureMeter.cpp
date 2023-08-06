// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 s-c-h-m-i-t-t
 */
// https://github.com/PaulStoffregen/OneWire
// https://github.com/stickbreaker/OneWire
// https://github.com/lcapossio/fresca/tree/master


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

      _lastTemperatureMeterCheck = millis();

      readTemperatureMeter();
  }
}

void TemperatureMeterClass::readTemperatureMeter(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //MessageOutput.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //MessageOutput.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  TempVal = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (TempVal != DEVICE_DISCONNECTED_C)
  {
    //MessageOutput.print("Temperature for the device 1 (index 0) is: ");
    //MessageOutput.println(TempVal);
  }
  else
  {
    MessageOutput.println("Error: Could not read temperature data");
  }
}


