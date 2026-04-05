#pragma once
#include <WiFi.h>
#include "coffee.h"
#include "http_server.hpp"

#define API_ERR_T int

API_ERR_T api_apply(WiFiClient* client, IFaceRegs* input, IFaceRegs* state);
API_ERR_T api_reply(WiFiClient *client, IFaceRegs *state);