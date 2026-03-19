#include "api.hpp"
#include "http_server.hpp"

API_ERR_T api_reply(WiFiClient *client, IFaceRegs *state) {
  char itoabuf[16];
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: application/json");
  client->println("Connection: close");
  client->println();
  client->print("{ \"TEMP\": ");
  itoa(state->temp, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"WVOT\": ");
  itoa(state->wvot, itoabuf, 10);
  client->print(itoabuf);
  
  client->print(", \"GDPB\": ");
  itoa(state->gdpb, itoabuf, 10);
  client->print(itoabuf);
  
  client->print(", \"SBTH\": ");
  itoa(state->sbth, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"SBTM\": ");
  itoa(state->sbtm, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"SBTS\": ");
  itoa(state->sbts, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"SBTD\": ");
  itoa(state->sbtd, itoabuf, 10);
  client->print(itoabuf);

  client->print(", \"CTRL\": ");
  itoa(state->ctrl, itoabuf, 2);
  client->print(itoabuf);
  client->println(" }");

  client->println();

  return 0;
}

API_ERR_T api_apply(WiFiClient *client, IFaceRegs *input, IFaceRegs *state) {
  // Validate input pointers
  if (input == NULL || state == NULL) {
    return ERR_NULL_POINTER;
  }

  // Copy all fields from input to state
  state->temp = input->temp;
  state->wvot = input->wvot;
  state->gdpb = input->gdpb;
  state->sbth = input->sbth;
  state->sbtm = input->sbtm;
  state->sbts = input->sbts;
  state->sbtd = input->sbtd;
  state->ctrl = input->ctrl;

  return 0;  // Success
}

/*
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
*/