#include <Wire.h>
#include <Arduino.h>
#include "coffee.h"
#include "i2c_comm.h"

#define I2C_SLAVE_ADDR 0x42
#define SDA_PIN 6
#define SCL_PIN 7

#define I2C_CMD_NONE 0
#define I2C_CMD_SET  1
#define I2C_CMD_PUT  2
#define I2C_CMD_GET  3

uint8_t I2C_ADDR = 0;
uint8_t I2C_LAST_STATUS = 0; // 0 = OK, 1 = ERR
uint8_t I2C_LAST_CMD = I2C_CMD_NONE;

//--------------------------------------------

void writeOK() {
  Wire.write('O');
  Wire.write('K');
  Wire.write('!');
  Wire.write(0x00);
}

void writeERR() {
  Wire.write('E');
  Wire.write('R');
  Wire.write('R');
  Wire.write(0x00);
}

//--------------------------------------------

bool isValidRegister(uint8_t reg) {
  switch (reg) {
    case REG_TEMP_HI:
    case REG_TEMP_LO:
    case REG_WVOT_HI:
    case REG_WVOT_LO:
    case REG_GDPB_HI:
    case REG_GDPB_LO:
    case REG_SBTH:
    case REG_SBTM:
    case REG_SBTS:
    case REG_SBTD:
    case REG_CTRL:
      return true;
    default:
      return false;
  }
}

//--------------------------------------------

void handlePut() {
  if (!isValidRegister(I2C_ADDR)) {
    I2C_LAST_STATUS = 1;
    return;
  }

  if (!Wire.available()) {
    I2C_LAST_STATUS = 1;
    return;
  }

  uint8_t b = Wire.read();

  switch (I2C_ADDR) {

    case REG_TEMP_HI:
      IFREGS.temp = (IFREGS.temp & 0x00FF) | (b << 8);
      break;

    case REG_TEMP_LO:
      IFREGS.temp = (IFREGS.temp & 0xFF00) | b;
      break;

    case REG_WVOT_HI:
      IFREGS.wvot = (IFREGS.wvot & 0x00FF) | (b << 8);
      break;

    case REG_WVOT_LO:
      IFREGS.wvot = (IFREGS.wvot & 0xFF00) | b;
      break;

    case REG_GDPB_HI:
      IFREGS.gdpb = (IFREGS.gdpb & 0x00FF) | (b << 8);
      break;

    case REG_GDPB_LO:
      IFREGS.gdpb = (IFREGS.gdpb & 0xFF00) | b;
      break;

    case REG_SBTH:
      IFREGS.sbth = b;
      break;

    case REG_SBTM:
      IFREGS.sbtm = b;
      break;

    case REG_SBTS:
      IFREGS.sbts = b;
      break;

    case REG_SBTD:
      IFREGS.sbtd = b;
      break;

    case REG_CTRL:
      IFREGS.ctrl = b;
      break;
  }

  I2C_LAST_STATUS = 0;
}

//--------------------------------------------

void handleGet() {
  static int sw = 0;
  if (!isValidRegister(I2C_ADDR)) {
    I2C_LAST_STATUS = 1;
    sw = !sw;
    if(sw) {
      Wire.write(0xDE);
    }
    else {
      Wire.write(0xAD);
    }
    return;
  }

  switch (I2C_ADDR++) {
    case REG_TEMP_HI:
      Wire.write((uint8_t)(IFREGS.temp >> 8));
      break;

    case REG_TEMP_LO:
      Wire.write((uint8_t)(IFREGS.temp & 0xFF));
      break;

    case REG_WVOT_HI:
      Wire.write((uint8_t)(IFREGS.wvot >> 8));
      break;

    case REG_WVOT_LO:
      Wire.write((uint8_t)(IFREGS.wvot & 0xFF));
      break;

    case REG_GDPB_HI:
      Wire.write((uint8_t)(IFREGS.gdpb >> 8));
      break;

    case REG_GDPB_LO:
      Wire.write((uint8_t)(IFREGS.gdpb & 0xFF));
      break;

    case REG_SBTH:
      Wire.write(IFREGS.sbth);
      break;

    case REG_SBTM:
      Wire.write(IFREGS.sbtm);
      break;

    case REG_SBTS:
      Wire.write(IFREGS.sbts);
      break;

    case REG_SBTD:
      Wire.write(IFREGS.sbtd);
      break;

    case REG_CTRL:
      Wire.write(IFREGS.ctrl);
      break;
  }

  I2C_LAST_STATUS = 0;
}

//--------------------------------------------

void receiveEvent(int numBytes) {
  if (!Wire.available()) return;

  uint8_t cmd = Wire.read();

  switch (cmd) {

    case I2C_CMD_NONE:
      I2C_LAST_STATUS = 0;
      break;

    case I2C_CMD_SET:
      if (Wire.available()) {
        uint8_t addr = Wire.read();
        if (isValidRegister(addr)) {
          I2C_ADDR = addr;
          I2C_LAST_STATUS = 0;
        } else {
          I2C_LAST_STATUS = 1;
        }
      } else {
        I2C_LAST_STATUS = 1;
      }
      break;

    case I2C_CMD_PUT:
      handlePut();
      break;

    case I2C_CMD_GET:
      // handled in requestEvent()
      break;

    default:
      I2C_LAST_STATUS = 1;
      break;
  }

  // Drain buffer safely
  while (Wire.available()) {
    Wire.read();
  }
}

//--------------------------------------------

void requestEvent() {
  if (I2C_LAST_STATUS != 0) {
    writeERR();
    return;
  }

  // Default behavior: return register value
  handleGet();
}

//--------------------------------------------

void i2c_init() {
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Wire.begin(I2C_SLAVE_ADDR, SDA_PIN, SCL_PIN, 400000);
  Wire.setBufferSize(32);

  Serial.print("I2C Slave ready at 0x");
  Serial.println(I2C_SLAVE_ADDR, HEX);
}

//--------------------------------------------

#undef I2C_SLAVE_ADDR
#undef SDA_PIN
#undef SCL_PIN