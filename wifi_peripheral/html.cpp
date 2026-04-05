#include "html.hpp"

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
  char itoabuf[80];

  client->println("<form id=\"configForm\">");

  itoa(IFREGS.temp, itoabuf, 10);
  rpl_html_input(client, "number", "temp", "Temp", itoabuf);

  itoa(IFREGS.wvot, itoabuf, 10);
  rpl_html_input(client, "number", "wvot", "Water Valve Open Time", itoabuf);

  itoa(IFREGS.gdpb, itoabuf, 10);
  rpl_html_input(client, "number", "gdpb", "Ground Weight Dispensed Per Brew", itoabuf);

  itoa(IFREGS.sbth, itoabuf, 10);
  rpl_html_input(client, "number", "sbth", "Scheduled Brew Time (Hour)", itoabuf);

  itoa(IFREGS.sbtm, itoabuf, 10);
  rpl_html_input(client, "number", "sbtm", "Scheduled Brew Time (Minute)", itoabuf);

  itoa(IFREGS.sbts, itoabuf, 10);
  rpl_html_input(client, "number", "sbts", "Scheduled Brew Time (Second)", itoabuf);

  itoa(IFREGS.sbtd, itoabuf, 10);
  rpl_html_input(client, "number", "sbtd", "Scheduled Brew Time (Days of the Week (bitfield))", itoabuf);

  itoa(IFREGS.ctrl, itoabuf, 10);
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
  char itoabuf[80];

  client->println("<table>");
  client->println("<tr>");
  client->println("<th>REG</th>");
  client->println("<th>NAME</th>");
  client->println("<th>VAL</th>");
  client->println("</tr>");

  itoa(IFREGS.temp, itoabuf, 10);
  rpl_html_reg(client, "0x00-0x01", "TEMP", itoabuf);

  itoa(IFREGS.wvot, itoabuf, 10);
  rpl_html_reg(client, "0x02-0x03", "WVOT", itoabuf);

  itoa(IFREGS.gdpb, itoabuf, 10);
  rpl_html_reg(client, "0x04-0x05", "GDPB", itoabuf);

  itoa(IFREGS.sbth, itoabuf, 10);
  rpl_html_reg(client, "0x06", "SBTH", itoabuf);

  itoa(IFREGS.sbtm, itoabuf, 10);
  rpl_html_reg(client, "0x07", "SBTM", itoabuf);

  itoa(IFREGS.sbts, itoabuf, 10);
  rpl_html_reg(client, "0x08", "SBTS", itoabuf);

  itoa(IFREGS.sbtd, itoabuf, 10);
  rpl_html_reg(client, "0x09", "SBTD", itoabuf);

  itoa(IFREGS.ctrl, itoabuf, 2);
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