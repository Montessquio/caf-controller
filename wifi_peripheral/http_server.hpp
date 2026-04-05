#pragma once
#include <WiFi.h>
#include "coffee.h"

#define ACT_GET 0
#define ACT_PUT 1
#define ACT_TGL 2
#define ACT_SET 3

#define KWD_TEMP    0
#define KWD_WVOT    2
#define KWD_GDPB    4
#define KWD_SBTH    6
#define KWD_SBTM    7
#define KWD_SBTS    8
#define KWD_SBTD    9
#define KWD_CTRL    11

#define ERR_UNKNOWN_ERR       -1
#define ERR_NULL_POINTER      -2
#define ERR_EMPTY_STRING      -3
#define ERR_MISSING_ACT       -4
#define ERR_MISSING_PART      -5
#define ERR_MISSING_VALUE     -6
#define ERR_EMPTY_SEGMENT     -7
#define ERR_UNKNOWN_ACT       -8
#define ERR_UNKNOWN_CTRL      -9
#define ERR_PARSE_INT        -10
#define ERR_PARAM_NOT_FOUND  -11
#define ERR_EMPTY_PARAM      -12

int http_getLine(WiFiClient* client, String* buf);
int http_getCtrlParams(String* path, IFaceRegs* params);
void rpl_header_ok(WiFiClient* client);
void rpl_header_redirect(WiFiClient* client, const char* path);
void rpl_header_not_found(WiFiClient* client, const char* path);
void rpl_err(WiFiClient* client, char* trace, int err);
void rpl_end(WiFiClient* client);