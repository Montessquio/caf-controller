#include <Wire.h>
#include <WiFi.h>
#include <limits.h>

#include "http_server.hpp"
#include "api.hpp"

#define I2C_SLAVE_ADDR 0x42
#define SDA_PIN 6
#define SCL_PIN 7

WiFiServer server(80);

char* itoabuf;
IFaceRegs regs;

void setup() {
  itoabuf = (char*)malloc(80);
  for (int i = 0; i < 80; i++) itoabuf[i] = 0;

  Serial.begin(115200);
  Serial.println("Strike the Earth!");

  Wire.begin(I2C_SLAVE_ADDR, SDA_PIN, SCL_PIN, 400000);

  Wire.setBufferSize(32);

  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.print("I2C Slave ready at 0x");
  Serial.println(I2C_SLAVE_ADDR, HEX);

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

void rpl_header_ok(WiFiClient* client) {
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connection: close");
  client->println();
}

void rpl_header_redirect(WiFiClient* client, const char* path) {
  client->println("HTTP/1.1 301 Moved Permanently");
  client->print("location: ");
  client->println(path);
  client->println("Content-Type: text/plain");
  client->println("Content-Length: 0");
  client->println("Connection: close");
  client->println();
}

void rpl_header_not_found(WiFiClient* client, const char* path) {
  client->println("HTTP/1.1 404 Not Found");
  client->print("location: ");
  client->println(path);
  client->println("Content-Type: text/plain");
  client->println("Content-Length: 0");
  client->println("Connection: close");
  client->println();
}

void rpl_html_input(WiFiClient* client, char* type, char* id, char* name, char* currentValue) {
  client->print("<input type=\"");
  client->print(type);
  client->print("\" id=\"");
  client->print(id);
  client->print("\" name=\"");
  client->print(id);
  client->print("\" value=\"");
  client->print(currentValue);
  client->println("\" />");

  client->print("<label for=\"");
  client->print(id);
  client->print("\">");
  client->print(name);
  client->println("</label>");

  client->println("<br />");
}

void rpl_html_btn(WiFiClient* client, char* type, char* id, char* name) {
  client->print("<button type=\"");
  client->print(type);
  client->print("\" id=\"");
  client->print(id);
  client->print("\" >");
  client->print(name);
  client->println("</button>");
}

void rpl_html_form(WiFiClient* client) {
  client->println("<form id=\"configForm\">");

  itoa(regs.temp, itoabuf, 10);
  rpl_html_input(client, "number", "temp", "Temp", itoabuf);

  itoa(regs.wvot, itoabuf, 10);
  rpl_html_input(client, "number", "wvot", "Water Valve Open Time", itoabuf);

  itoa(regs.gdpb, itoabuf, 10);
  rpl_html_input(client, "number", "gdpb", "Ground Weight Dispensed Per Brew", itoabuf);

  itoa(regs.sbth, itoabuf, 10);
  rpl_html_input(client, "number", "sbth", "Scheduled Brew Time (Hour)", itoabuf);

  itoa(regs.sbtm, itoabuf, 10);
  rpl_html_input(client, "number", "sbtm", "Scheduled Brew Time (Minute)", itoabuf);

  itoa(regs.sbts, itoabuf, 10);
  rpl_html_input(client, "number", "sbts", "Scheduled Brew Time (Second)", itoabuf);

  itoa(regs.sbtd, itoabuf, 10);
  rpl_html_input(client, "number", "sbtd", "Scheduled Brew Time (Days of the Week (bitfield))", itoabuf);

  itoa(regs.ctrl, itoabuf, 10);
  rpl_html_input(client, "number", "ctrl", "Control Register (bitfield)", itoabuf);

  rpl_html_btn(client, "submit", "btnSubmit", "Submit");
}
 
void rpl_html_reg(WiFiClient* client, char* offset, char* name, char* value) {
  client->println("<tr>");
  client->println("<td>");
  client->println(offset);
  client->println("</td>");
  client->println("<td>");
  client->println(name);
  client->println("</td>");
  client->println("<td>");
  client->println(value);
  client->println("</td>");
  client->println("</tr>");
}

void rpl_html_regs(WiFiClient* client) {
  client->println("<table>");
  client->println("<tr>");
  client->println("<th>REG</th>");
  client->println("<th>NAME</th>");
  client->println("<th>VAL</th>");
  client->println("</tr>");

  itoa(regs.temp, itoabuf, 10);
  rpl_html_reg(client, "0x00-0x01", "TEMP", itoabuf);

  itoa(regs.wvot, itoabuf, 10);
  rpl_html_reg(client, "0x02-0x03", "WVOT", itoabuf);

  itoa(regs.gdpb, itoabuf, 10);
  rpl_html_reg(client, "0x04-0x05", "GDPB", itoabuf);

  itoa(regs.sbth, itoabuf, 10);
  rpl_html_reg(client, "0x06", "SBTH", itoabuf);

  itoa(regs.sbtm, itoabuf, 10);
  rpl_html_reg(client, "0x07", "SBTM", itoabuf);

  itoa(regs.sbts, itoabuf, 10);
  rpl_html_reg(client, "0x08", "SBTS", itoabuf);

  itoa(regs.sbtd, itoabuf, 10);
  rpl_html_reg(client, "0x09", "SBTD", itoabuf);

  itoa(regs.ctrl, itoabuf, 2);
  rpl_html_reg(client, "0x0A", "CTRL", itoabuf);

  client->println("</table>");
}

void rpl_html(WiFiClient* client, String* path) {
  client->println("<!DOCTYPE HTML>");
  client->println("<html>");
  client->println("<head><title>SMART Coffee UI</title></head>");
  client->println("<body>");
  client->println("<h1>SMART Coffee UI</h1>");
  client->println("<p>Path: " + *path + "</p>");
  client->println("<p>Uptime: " + String(millis() / 1000) + " seconds</p>");
  rpl_html_form(client);
  client->println("<br />");
  rpl_html_regs(client);
  client->println("</body>");
  client->println("</html>");
}

void rpl_err(WiFiClient* client, char* trace, int err) {
  client->print(trace);
  itoa(err, itoabuf, 10);
  client->println(itoabuf);
  client->stop();
}

void rpl_end(WiFiClient* client) {
  client->println();
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
      rpl_err(client, "ECPRM", result);
    } else {

      if ((result = api_apply(client, &params, &regs)) != 0) {
        rpl_err(client, "ECPRM", result);
      }
      
      rpl_header_ok(client);
      rpl_html(client, path);
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

void receiveEvent(int numBytes) {
  Serial.print("<< (");
  Serial.print(numBytes);
  Serial.print(")");

  Serial.print("0x");
  while (Wire.available()) {
    uint8_t b = Wire.read();
    Serial.print(b, HEX);
  }
  Serial.println();
}

void requestEvent() {
  Serial.print(">>");
  Wire.write('R');
  Wire.write('P');
  Wire.write('L');
  Wire.write(0x00);
}