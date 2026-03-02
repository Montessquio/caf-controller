#include <Wire.h>

#define I2C_SLAVE_ADDR 0x42
#define SDA_PIN 6
#define SCL_PIN 7

void setup() {
  Serial.begin(115200);
  Serial.println("Strike the Earth!");
  
  Wire.begin(I2C_SLAVE_ADDR, SDA_PIN, SCL_PIN, 400000);
  
  Wire.setBufferSize(32);
  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  Serial.print("I2C Slave ready at 0x");
  Serial.println(I2C_SLAVE_ADDR, HEX);
  Serial.println("Waiting for data...");
}

void loop() {
  delay(1000);
}

void receiveEvent(int numBytes) {
  Serial.print("<< (");
  Serial.print(numBytes);
  Serial.prin(")");

  Serial.print("0x");
  while (Wire.available()) {
    uint8_t b = Wire.read();
    Serial.print(b, HEX);
  }
  Serial.println();
}

void requestEvent() {
  Serial.println("Request event");
  Wire.write('R');
  Wire.write('P');
  Wire.write('L');
  Wire.write(0x00);
}