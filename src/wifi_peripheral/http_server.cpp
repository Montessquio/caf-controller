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
  return n;
}

int http_getQueryParam(String* queryString, const char* paramName, String& output) {
  Serial.print("Extracting ");
  Serial.println(paramName);
  
  if (queryString == NULL || queryString->length() == 0) {
    return ERR_EMPTY_STRING;
  }

  // Find the parameter name in the query string
  String searchKey = String(paramName) + "=";
  int paramStart = queryString->indexOf(searchKey);
  
  if (paramStart == -1) {
    return ERR_PARAM_NOT_FOUND;  // Parameter not found
  }

  // Move past the parameter name and '='
  paramStart += searchKey.length();

  // Find the end of the parameter value (next '&' or end of string)
  int paramEnd = queryString->indexOf('&', paramStart);
  
  if (paramEnd == -1) {
    // This is the last parameter
    output = queryString->substring(paramStart);
  } else {
    output = queryString->substring(paramStart, paramEnd);
  }

  // Check for empty value
  if (output.length() == 0) {
    return ERR_EMPTY_PARAM;
  }

  return 0;
}

int http_getCtrlParams(String* queryString, IFaceRegs* params) {
  // Validate input
  if (queryString == NULL) return ERR_NULL_POINTER;
  if (params == NULL) return ERR_NULL_POINTER;
  if (queryString->length() == 0) return ERR_EMPTY_STRING;

  String value;

  // Parse temp (uint16_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "temp", value));
  if (!parseIntSafe(&value, &(params->temp))) return ERR_PARSE_INT;

  // Parse wvot (uint16_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "wvot", value));
  if (!parseIntSafe(&value, &(params->wvot))) return ERR_PARSE_INT;

  // Parse gdpb (uint16_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "gdpb", value));
  if (!parseIntSafe(&value, &(params->gdpb))) return ERR_PARSE_INT;

  // Parse sbth (uint8_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "sbth", value));
  if (!parseIntSafe(&value, &(params->sbth))) return ERR_PARSE_INT;

  // Parse sbtm (uint8_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "sbtm", value));
  if (!parseIntSafe(&value, &(params->sbtm))) return ERR_PARSE_INT;

  // Parse sbts (uint8_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "sbts", value));
  if (!parseIntSafe(&value, &(params->sbts))) return ERR_PARSE_INT;

  // Parse sbtd (uint8_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "sbtd", value));
  if (!parseIntSafe(&value, &(params->sbtd))) return ERR_PARSE_INT;

  // Parse ctrl (uint8_t)
  HTTP_RET_IF_ERR(http_getQueryParam(queryString, "ctrl", value));
  if (!parseIntSafe(&value, &(params->ctrl))) return ERR_PARSE_INT;

  return 0;
}

#undef HTTP_RET_IF_ERR