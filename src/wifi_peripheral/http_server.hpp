#pragma once
#include <WiFi.h>

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

#define REG_TEMP_HI 0
#define REG_TEMP_LO 1
#define REG_WVOT_HI 2
#define REG_WVOT_LO 3
#define REG_GDPB_HI 4
#define REG_GDPB_LO 5
#define REG_SBTH    6
#define REG_SBTM    7
#define REG_SBTS    8
#define REG_SBTD    9
#define REG_CTRL    11

#define MASK_CTRL_BREW_START  1 << 0;
#define MASK_CTRL_BREW_PAUSE  1 << 1;
#define MASK_CTRL_GRIND_START 1 << 2;
#define MASK_CTRL_GRIND_PAUSE 1 << 3;
#define MASK_CTRL_RESRV_HEAT  1 << 4;
#define MASK_CTRL_CLEAN_START 1 << 7;

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

#define u16_write(s, v) (uint16_t* s)

struct IFaceRegs {
  /// Temperature to keep the
  /// reservoir at.
  uint16_t temp;

  /// Water valve open-time (ms)
  uint16_t wvot;

  /// Amount of grounds to dispense
  /// per brew (sensor unit)
  uint16_t gdpb;
  
  /// Scheduled brew time (H, M, S)
  uint8_t sbth;
  uint8_t sbtm;
  uint8_t sbts;
  uint8_t sbtd; // Days of the week (bitfield)

  /// Control bitflags (as 1 / 0), write-only.
  /// Bit 0: Start/Stop Brew
  /// Bit 1: Pause/Resume Brew
  /// Bit 2: Start/Stop Grind
  /// Bit 3: Pause/Resume Grind
  /// Bit 4: Pause/Resume Reservoir Heating
  /// Bit 5-6: Reserved
  /// Bit 7: Start Clean Cycle
  uint8_t ctrl;
};

int http_getLine(WiFiClient* client, String* buf);
int http_getCtrlParams(String* path, IFaceRegs* params);