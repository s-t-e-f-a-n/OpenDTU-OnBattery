// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_ws_vedirect2_live.h"
#include "AsyncJson.h"
#include "Configuration.h"
#include "MessageOutput.h"
#include "WebApi.h"
#include "defaults.h"
#include "PowerLimiter.h"

WebApiWsVedirect2LiveClass::WebApiWsVedirect2LiveClass()
    : _ws("/vedirect2livedata")
{
}

void WebApiWsVedirect2LiveClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;
    using std::placeholders::_5;
    using std::placeholders::_6;

    _server = server;
    _server->on("/api/vedirect2livedata/status", HTTP_GET, std::bind(&WebApiWsVedirect2LiveClass::onLivedataStatus, this, _1));

    _server->addHandler(&_ws);
    _ws.onEvent(std::bind(&WebApiWsVedirect2LiveClass::onWebsocketEvent, this, _1, _2, _3, _4, _5, _6));
}

void WebApiWsVedirect2LiveClass::loop()
{
    // see: https://github.com/me-no-dev/ESPAsyncWebServer#limiting-the-number-of-web-socket-clients
    if (millis() - _lastWsCleanup > 1000) {
        _ws.cleanupClients();
        _lastWsCleanup = millis();
    }

    // do nothing if no WS client is connected
    if (_ws.count() == 0) {
        return;
    }

    if (millis() - _lastVedirectUpdateCheck < 1000) {
        return;
    }
    _lastVedirectUpdateCheck = millis();

    uint32_t maxTimeStamp = 0;
    if (VeDirect2.getLastUpdate() > maxTimeStamp) {
        maxTimeStamp = VeDirect2.getLastUpdate();
    }

    // Update on ve.direct change or at least after 10 seconds
    if (millis() - _lastWsPublish > (10 * 1000) || (maxTimeStamp != _newestVedirectTimestamp)) {
        
        try {
            String buffer;
            // free JsonDocument as soon as possible
            {
                DynamicJsonDocument root(2048);
                JsonVariant var = root;
                generateJsonResponse(var);
                serializeJson(root, buffer);
            }
            
            if (buffer) {        
                if (Configuration.get().Security_AllowReadonly) {
                    _ws.setAuthentication("", "");
                } else {
                    _ws.setAuthentication(AUTH_USERNAME, Configuration.get().Security_Password);
                }

                _ws.textAll(buffer);
            }

        } catch (std::bad_alloc& bad_alloc) {
            MessageOutput.printf("Calling /api/vedirect2livedata/status has temporarily run out of resources. Reason: \"%s\".\r\n", bad_alloc.what());
        }

        _lastWsPublish = millis();
    }
}

void WebApiWsVedirect2LiveClass::generateJsonResponse(JsonVariant& root)
{
    // device info
    root["device"]["data_age"] = (millis() - VeDirect2.getLastUpdate() ) / 1000;
    root["device"]["age_critical"] = !VeDirect2.isDataValid();
    root["device"]["PID"] = VeDirect2.getPidAsString(VeDirect2.veFrame.PID);
    root["device"]["SER"] = VeDirect2.veFrame.SER;
    root["device"]["FW"] = VeDirect2.veFrame.FW;
    root["device"]["LOAD"] = VeDirect2.veFrame.LOAD == true ? "ON" : "OFF";
    root["device"]["CS"] = VeDirect2.getCsAsString(VeDirect2.veFrame.CS);
    root["device"]["ERR"] = VeDirect2.getErrAsString(VeDirect2.veFrame.ERR);
    root["device"]["OR"] = VeDirect2.getOrAsString(VeDirect2.veFrame.OR);
    root["device"]["MPPT"] = VeDirect2.getMpptAsString(VeDirect2.veFrame.MPPT);
    root["device"]["HSDS"]["v"] = VeDirect2.veFrame.HSDS;
    root["device"]["HSDS"]["u"] = "d";

    // battery info    
    root["output"]["P"]["v"] = VeDirect2.veFrame.P;
    root["output"]["P"]["u"] = "W";
    root["output"]["P"]["d"] = 0;
    root["output"]["V"]["v"] = VeDirect2.veFrame.V;
    root["output"]["V"]["u"] = "V";
    root["output"]["V"]["d"] = 2;
    root["output"]["I"]["v"] = VeDirect2.veFrame.I;
    root["output"]["I"]["u"] = "A";
    root["output"]["I"]["d"] = 2;
    root["output"]["E"]["v"] = VeDirect2.veFrame.E;
    root["output"]["E"]["u"] = "%";
    root["output"]["E"]["d"] = 1;

    // panel info
    root["input"]["PPV"]["v"] = VeDirect2.veFrame.PPV;
    root["input"]["PPV"]["u"] = "W";
    root["input"]["PPV"]["d"] = 0;
    root["input"]["VPV"]["v"] = VeDirect2.veFrame.VPV;
    root["input"]["VPV"]["u"] = "V";
    root["input"]["VPV"]["d"] = 2;
    root["input"]["IPV"]["v"] = VeDirect2.veFrame.IPV;
    root["input"]["IPV"]["u"] = "A";
    root["input"]["IPV"]["d"] = 2;
    root["input"]["YieldToday"]["v"] = VeDirect2.veFrame.H20;
    root["input"]["YieldToday"]["u"] = "kWh";
    root["input"]["YieldToday"]["d"] = 3;
    root["input"]["YieldYesterday"]["v"] = VeDirect2.veFrame.H22;
    root["input"]["YieldYesterday"]["u"] = "kWh";
    root["input"]["YieldYesterday"]["d"] = 3;
    root["input"]["YieldTotal"]["v"] = VeDirect2.veFrame.H19;
    root["input"]["YieldTotal"]["u"] = "kWh";
    root["input"]["YieldTotal"]["d"] = 3;
    root["input"]["MaximumPowerToday"]["v"] = VeDirect2.veFrame.H21;
    root["input"]["MaximumPowerToday"]["u"] = "W";
    root["input"]["MaximumPowerToday"]["d"] = 0;
    root["input"]["MaximumPowerYesterday"]["v"] = VeDirect2.veFrame.H23;
    root["input"]["MaximumPowerYesterday"]["u"] = "W";
    root["input"]["MaximumPowerYesterday"]["d"] = 0;

    // power limiter state
    root["dpl"]["PLSTATE"] = -1;
    if (Configuration.get().PowerLimiter_Enabled)
        root["dpl"]["PLSTATE"] = PowerLimiter.getPowerLimiterState();
    root["dpl"]["PLLIMIT"] = PowerLimiter.getLastRequestedPowerLimit();

    if (VeDirect2.getLastUpdate() > _newestVedirectTimestamp) {
        _newestVedirectTimestamp = VeDirect2.getLastUpdate();
    }
}

void WebApiWsVedirect2LiveClass::onWebsocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
{
    if (type == WS_EVT_CONNECT) {
        char str[64];
        snprintf(str, sizeof(str), "Websocket: [%s][%u] connect", server->url(), client->id());
        Serial.println(str);
        MessageOutput.println(str);
    } else if (type == WS_EVT_DISCONNECT) {
        char str[64];
        snprintf(str, sizeof(str), "Websocket: [%s][%u] disconnect", server->url(), client->id());
        Serial.println(str);
        MessageOutput.println(str);
    }
}

void WebApiWsVedirect2LiveClass::onLivedataStatus(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentialsReadonly(request)) {
        return;
    }
    try {
        AsyncJsonResponse* response = new AsyncJsonResponse(false, 1024U);
        JsonVariant root = response->getRoot();

        generateJsonResponse(root);

        response->setLength();
        request->send(response);

    } catch (std::bad_alloc& bad_alloc) {
        MessageOutput.printf("Calling /api/livedata/status has temporarily run out of resources. Reason: \"%s\".\r\n", bad_alloc.what());

        WebApi.sendTooManyRequests(request);
    }
}