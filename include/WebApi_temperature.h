// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ESPAsyncWebServer.h>


class WebApiTemperatureClass {
public:
    void init(AsyncWebServer* server);
    void loop();

private:
    void onTemperatureStatus(AsyncWebServerRequest* request);
    void onTemperatureAdminGet(AsyncWebServerRequest* request);
    void onTemperatureAdminPost(AsyncWebServerRequest* request);

    AsyncWebServer* _server;
};