#include <WiFi.h>
#include <limits.h>
#include "coffee.h"
#include "http_server.hpp"
#include "i2c_comm.h"
#include "html.hpp"
#include "api.hpp"

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Strike the Earth!");

  i2c_init();

  WiFi.begin("The Way of the Wood", "a forgotten chronicle 6");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin();
  IPAddress myAddress = WiFi.localIP();
  Serial.print("Connected to wifi. My address:");
  Serial.println(myAddress);
}

int rpl_handle(WiFiClient* client, String* path) {
  const char * cpath = path->c_str();

  // Handle root
  if (strlen(cpath) == 0 || strcmp(cpath, "/") == 0) {
    Serial.println("Got ROOT");
    rpl_header_ok(client);
    rpl_html(client, path);
    rpl_end(client);
    return 0;
  }
  
  // Handle data submission
  if(strncmp("/?", cpath, strlen("/?")) == 0){
    Serial.println("Got PARAM");
    IFaceRegs params;
    
    int result;
    if ((result = http_getCtrlParams(path, &params)) != 0) {
      rpl_err(client, "GTPRM", result);
      rpl_end(client);
      return 0;
    } else {
      // Result was OK
      if ((result = api_apply(client, &params, &IFREGS)) != 0) {
        rpl_err(client, "APPRM", result);
        rpl_end(client);
        return 0;
      }
      
      // Redirect to / to remove the params and prevent a
      // page refresh from re-applying the parameter set.
      rpl_header_redirect(client, "/");
      rpl_end(client);
    }
    rpl_end(client);
    return 0;
  }

  // Handle everything else
  rpl_header_not_found(client, cpath);
  rpl_end(client);
  return 0;
}

void handle_client(WiFiClient client) {
  // String to hold incoming data
  String request = "";
  String path = "";

  // The request URI path is in the first header line.
  http_getLine(&client, &request);
  int firstSpace = request.indexOf(' ');
  int secondSpace = request.indexOf(' ', firstSpace + 1);
  path = request.substring(firstSpace + 1, secondSpace);

  // Skip all the rest of the request header
  while (client.connected() && client.available()) {
    http_getLine(&client, &request);
    if (request.endsWith("\r\n\r\n")) break;
  }

  Serial.write(request.c_str());

  rpl_handle(&client, &path);
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println("Connected to client");
    }
    handle_client(client);
    client.stop();
  }
}
