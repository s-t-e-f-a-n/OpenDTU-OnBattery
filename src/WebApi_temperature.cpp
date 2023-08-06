// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_temperature.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "Configuration.h"
#include "WebApi.h"
#include "WebApi_errors.h"
#include "helper.h"

void WebApiTemperatureClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;

    _server = server;

    _server->on("/api/temperature/status", HTTP_GET, std::bind(&WebApiTemperatureClass::onTemperatureStatus, this, _1));
    _server->on("/api/temperature/config", HTTP_GET, std::bind(&WebApiTemperatureClass::onTemperatureAdminGet, this, _1));
    _server->on("/api/temperature/config", HTTP_POST, std::bind(&WebApiTemperatureClass::onTemperatureAdminPost, this, _1));
}

void WebApiTemperatureClass::loop()
{
}

void WebApiTemperatureClass::onTemperatureStatus(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentialsReadonly(request)) {
        return;
    }
    
    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();
    const CONFIG_T& config = Configuration.get();

    root[F("temperature_enabled")] = config.Temperature_Enabled;
    root[F("temperature_updatesonly")] = config.Temperature_UpdatesOnly;

    response->setLength();
    request->send(response);
}

void WebApiTemperatureClass::onTemperatureAdminGet(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();
    const CONFIG_T& config = Configuration.get();

    root[F("temperature_enabled")] = config.Temperature_Enabled;
    root[F("temperature_updatesonly")] = config.Temperature_UpdatesOnly;

    response->setLength();
    request->send(response);
}

void WebApiTemperatureClass::onTemperatureAdminPost(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject retMsg = response->getRoot();
    retMsg[F("type")] = F("warning");

    if (!request->hasParam("data", true)) {
        retMsg[F("message")] = F("No values found!");
        retMsg[F("code")] = WebApiError::GenericNoValueFound;
        response->setLength();
        request->send(response);
        return;
    }

    String json = request->getParam("data", true)->value();

    if (json.length() > 1024) {
        retMsg[F("message")] = F("Data too large!");
        retMsg[F("code")] = WebApiError::GenericDataTooLarge;
        response->setLength();
        request->send(response);
        return;
    }

    DynamicJsonDocument root(1024);
    DeserializationError error = deserializeJson(root, json);

    if (error) {
        retMsg[F("message")] = F("Failed to parse data!");
        retMsg[F("code")] = WebApiError::GenericParseError;
        response->setLength();
        request->send(response);
        return;
    }

    if (!(root.containsKey("temperature_enabled") && root.containsKey("temperature_updatesonly")) ) {
        retMsg[F("message")] = F("Values are missing!");
        retMsg[F("code")] = WebApiError::GenericValueMissing;
        response->setLength();
        request->send(response);
        return;
    }

    CONFIG_T& config = Configuration.get();
    config.Temperature_Enabled = root[F("temperature_enabled")].as<bool>();
    config.Temperature_UpdatesOnly = root[F("temperature_updatesonly")].as<bool>();
    Configuration.write();

    retMsg[F("type")] = F("success");
    retMsg[F("message")] = F("Settings saved!");
    retMsg[F("code")] = WebApiError::GenericSuccess;

    response->setLength();
    request->send(response);
}
