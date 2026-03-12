#include "api.hpp"
#include "http_server.hpp"

API_ERR_T api_read(uint8_t field, IFaceRegs *state, uint16_t *out) {
  switch (field) {
  case KWD_TEMP:
    *out = state->TEMP;
    return 0;
  case KWD_WVOT:
    *out = state->WVOT;
    return 0;
  case KWD_GDPB:
    *out = state->GDPB;
    return 0;
  case KWD_SBTH:
    *out = state->SBTH;
    return 0;
  case KWD_SBTM:
    *out = state->SBTM;
    return 0;
  case KWD_SBTS:
    *out = state->SBTS;
    return 0;
  case KWD_SBTD:
    *out = state->SBTD;
    return 0;
  case KWD_CTRL:
    *out = state->CTRL;
    return 0;
  default:
    return ERR_UNKNOWN_CTRL;
  }
}

API_ERR_T api_write(uint8_t field, uint16_t value, IFaceRegs *state) {
  switch (field) {
  case KWD_TEMP:
    state->TEMP = value;
    return 0;
  case KWD_WVOT:
    state->WVOT = value;
    return 0;
  case KWD_GDPB:
    state->GDPB = value;
    return 0;
  case KWD_SBTH:
    if (value > 255)
      return ERR_PARSE_INT;
    state->SBTH = value;
    return 0;
  case KWD_SBTM:
    if (value > 255)
      return ERR_PARSE_INT;
    state->SBTM = value;
    return 0;
  case KWD_SBTS:
    if (value > 255)
      return ERR_PARSE_INT;
    state->SBTS = value;
    return 0;
  case KWD_SBTD:
    if (value > 255)
      return ERR_PARSE_INT;
    state->SBTD = value;
    return 0;
  case KWD_CTRL:
    if (value > 255)
      return ERR_PARSE_INT;
    state->CTRL = value;
    return 0;
  default:
    return ERR_UNKNOWN_CTRL;
  }
}

API_ERR_T api_reply(WiFiClient *client, IFaceRegs *state) {
  char itoabuf[16];
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: application/json");
  client->println("Connection: close");
  client->println();
  client->print("{ \"TEMP\": ");
  itoa(state->TEMP, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"WVOT\": ");
  itoa(state->WVOT, itoabuf, 10);
  client->print(itoabuf);
  
  client->print(", \"GDPB\": ");
  itoa(state->GDPB, itoabuf, 10);
  client->print(itoabuf);
  
  client->print(", \"SBTH\": ");
  itoa(state->SBTH, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"SBTM\": ");
  itoa(state->SBTM, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"SBTS\": ");
  itoa(state->SBTS, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"SBTD\": ");
  itoa(state->SBTD, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"CTRL\": ");
  itoa(state->CTRL, itoabuf, 2);
  client->print(itoabuf);
  client->println(" }");

  client->println();

  return 0;
}

API_ERR_T api_apply(WiFiClient *client, CtrlParams *input, IFaceRegs *state) {
  API_ERR_T result;

  uint16_t oldval, newval;

  if ((result = api_read(input->CTRL, state, &oldval)) != 0) {
    return result;
  }

  switch (input->ACT) {
  case ACT_GET:
    newval = oldval;
    break;
  case ACT_PUT:
    newval = input->VALUE;
    break;
  case ACT_TGL:
    newval = oldval ^ input->VALUE;
    break;
  case ACT_SET:
    newval = oldval | input->VALUE;
    break;
  default:
    return ERR_UNKNOWN_ACT;
  }

  if ((result = api_write(input->CTRL, newval, state)) != 0) {
    return result;
  }
  
  return api_reply(client, state);
}

int http_parse_ctrl(String *ctrl, uint8_t *out) {
  if (*ctrl == "temp")
    *out = KWD_TEMP;
  else if (*ctrl == "wvot")
    *out = KWD_WVOT;
  else if (*ctrl == "gdpb")
    *out = KWD_GDPB;
  else if (*ctrl == "sbth")
    *out = KWD_SBTH;
  else if (*ctrl == "sbtm")
    *out = KWD_SBTM;
  else if (*ctrl == "sbts")
    *out = KWD_SBTS;
  else if (*ctrl == "sbtd")
    *out = KWD_SBTD;
  else if (*ctrl == "ctrl")
    *out = KWD_CTRL;
  else
    return ERR_UNKNOWN_CTRL;

  return 0;
}