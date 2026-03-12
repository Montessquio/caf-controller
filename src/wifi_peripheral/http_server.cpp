#include "http_server.hpp"

#define HTTP_RET_IF_ERR(expr) \
  do { \
    int _err = (expr); \
    if (_err != 0) { \
      return _err; \
    } \
  } while (0)

template<typename T>
bool parseIntSafe(String* str, T* output) {
  str->trim();
  if (str->length() == 0) return false;

  long value = str->toInt();

  if (value < (long)std::numeric_limits<T>::min() || value > (long)std::numeric_limits<T>::max()) {
    return false;
  }

  *output = (T)value;
  return true;
}

int http_getLine(WiFiClient* client, String* buf) {
  int n = 0;
  while (client->connected()) {
    if (client->available()) {
      n += 1;
      char c = client->read();
      (*buf) += c;

      if (c == '\n') {
        return n;
      }
    }
  }
}

int http_getSegment(String* path, int& startIndex, String& output, bool isLast = false) {
  if (startIndex >= path->length()) return -1;

  int endIndex = path->indexOf('/', startIndex);

  if (endIndex == -1 && !isLast) return -1;

  // Extract the segment
  if (endIndex == -1) {
    output = path->substring(startIndex);  // Get rest of string
  } else {
    output = path->substring(startIndex, endIndex);
  }

  // Check for empty segment
  if (output.length() == 0) return ERR_EMPTY_SEGMENT;

  startIndex = (endIndex == -1) ? path->length() : endIndex + 1;

  return 0;
}

int http_parse_act(String* act, uint8_t* out) {
  if (*act == "get") *out = ACT_GET;
  else if (*act == "put") *out = ACT_PUT;
  else if (*act == "tgl") *out = ACT_TGL;
  else if (*act == "set") *out = ACT_SET;
  else return ERR_UNKNOWN_ACT;

  return 0;
}

int http_parse_ctrl(String* ctrl, uint8_t* out) {
  if (*ctrl == "temp") *out = KWD_TEMP;
  else if (*ctrl == "wvot") *out = KWD_WVOT;
  else if (*ctrl == "gdpb") *out = KWD_GDPB;
  else if (*ctrl == "sbth") *out = KWD_SBTH;
  else if (*ctrl == "sbtm") *out = KWD_SBTM;
  else if (*ctrl == "sbts") *out = KWD_SBTS;
  else if (*ctrl == "sbtd") *out = KWD_SBTD;
  else if (*ctrl == "ctrl") *out = KWD_CTRL;
  else return ERR_UNKNOWN_CTRL;

  return 0;
}

int http_parse_value(String* value, uint8_t* ctrl, uint16_t* out) {
  switch (*ctrl) {
    case KWD_TEMP:
    case KWD_WVOT:
    case KWD_GDPB:
      {
        uint16_t p16 = 0;
        if (!parseIntSafe(value, &p16)) return ERR_PARSE_INT;
        *out = p16;
        break;
      }
    case KWD_SBTH:
    case KWD_SBTM:
    case KWD_SBTS:
    case KWD_SBTD:
    case KWD_CTRL:
      {
        uint8_t p8 = 0;
        if (!parseIntSafe(value, &p8)) return ERR_PARSE_INT;
        *out = p8;
        break;
      }
    default:
      return ERR_UNKNOWN_CTRL;
  }
}

int http_getCtrlParams(String* path, CtrlParams* params) {
  // Validate input
  if (path == NULL) return ERR_NULL_POINTER;
  if (path->length() == 0) return ERR_EMPTY_STRING;

  // Extract parameter values
  int startIndex = 0;
  String act, part, value;
  int result;
  HTTP_RET_IF_ERR(http_getSegment(path, startIndex, act));
  HTTP_RET_IF_ERR(http_getSegment(path, startIndex, part));
  HTTP_RET_IF_ERR(http_getSegment(path, startIndex, value, true));

  HTTP_RET_IF_ERR(http_parse_act(&act, &(params->ACT)));
  HTTP_RET_IF_ERR(http_parse_ctrl(&part, &(params->CTRL)));
  HTTP_RET_IF_ERR(http_parse_value(&value, &(params->ACT), &(params->VALUE)));

  return 0;
}

#undef HTTP_RET_IF_ERR
