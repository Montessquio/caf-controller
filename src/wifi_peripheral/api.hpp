#pragma once
#include <WiFi.h>
#include "http_server.hpp"

#define API_ERR_T int

API_ERR_T api_apply(WiFiClient* client, CtrlParams* input, IFaceRegs* state);
