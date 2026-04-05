#include <Wire.h>

#define I2C_SLAVE_ADDR 0x42

#define BREWING_LED         6
#define BREWING_PAUSED_LED  7
#define GRINDING_LED        8
#define GRINDING_PAUSED_LED 9
#define CLEANING_LED        10
#define HEATER_LED          11
#define HEATER_PAUSED_LED   12

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

#define I2C_CMD_NONE 0
#define I2C_CMD_SET  1
#define I2C_CMD_PUT  2
#define I2C_CMD_GET  3

#define MASK_CTRL_BREW_START  (1 << 0)
#define MASK_CTRL_BREW_PAUSE  (1 << 1)
#define MASK_CTRL_GRIND_START (1 << 2)
#define MASK_CTRL_GRIND_PAUSE (1 << 3)
#define MASK_CTRL_RESRV_HEAT  (1 << 4)
#define MASK_CTRL_CLEAN_START (1 << 7)

struct IFaceRegs {
  uint16_t temp;
  uint16_t wvot;
  uint16_t gdpb;

  uint8_t sbth;
  uint8_t sbtm;
  uint8_t sbts;
  uint8_t sbtd;

  uint8_t ctrl;
};

IFaceRegs IFREGS;

void i2c_set_reg(uint8_t reg) {
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(I2C_CMD_SET);
  Wire.write(reg);
  Wire.endTransmission();
}

uint8_t i2c_get_u8(uint8_t reg) {
  i2c_set_reg(reg);

  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(I2C_CMD_GET);
  Wire.endTransmission();

  Wire.requestFrom(I2C_SLAVE_ADDR, 1);

  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

uint16_t i2c_get_u16(uint8_t reg_hi, uint8_t reg_lo) {
  uint16_t hi = i2c_get_u8(reg_hi);
  uint16_t lo = i2c_get_u8(reg_lo);
  return (hi << 8) | lo;
}

void reloadIFaceRegs() {
  IFREGS.temp = i2c_get_u16(REG_TEMP_HI, REG_TEMP_LO);
  IFREGS.wvot = i2c_get_u16(REG_WVOT_HI, REG_WVOT_LO);
  IFREGS.gdpb = i2c_get_u16(REG_GDPB_HI, REG_GDPB_LO);

  IFREGS.sbth = i2c_get_u8(REG_SBTH);
  IFREGS.sbtm = i2c_get_u8(REG_SBTM);
  IFREGS.sbts = i2c_get_u8(REG_SBTS);
  IFREGS.sbtd = i2c_get_u8(REG_SBTD);

  IFREGS.ctrl = i2c_get_u8(REG_CTRL);
}

void printIFaceRegs() {
  Serial.println("---- IFACE REGS ----");

  Serial.print("TEMP  : "); Serial.println(IFREGS.temp);
  Serial.print("WVOT  : "); Serial.println(IFREGS.wvot);
  Serial.print("GDPB  : "); Serial.println(IFREGS.gdpb);

  Serial.print("TIME  : ");
  Serial.print(IFREGS.sbth); Serial.print(":");
  Serial.print(IFREGS.sbtm); Serial.print(":");
  Serial.println(IFREGS.sbts);

  Serial.print("DAYS  : 0b");
  Serial.println(IFREGS.sbtd, BIN);

  Serial.print("CTRL  : 0b");
  Serial.println(IFREGS.ctrl, BIN);

  Serial.print("BREW START : "); Serial.println(!!(IFREGS.ctrl & MASK_CTRL_BREW_START));
  Serial.print("BREW PAUSE : "); Serial.println(!!(IFREGS.ctrl & MASK_CTRL_BREW_PAUSE));
  Serial.print("GRIND START: "); Serial.println(!!(IFREGS.ctrl & MASK_CTRL_GRIND_START));
  Serial.print("GRIND PAUSE: "); Serial.println(!!(IFREGS.ctrl & MASK_CTRL_GRIND_PAUSE));
  Serial.print("HEAT       : "); Serial.println(!!(IFREGS.ctrl & MASK_CTRL_RESRV_HEAT));
  Serial.print("CLEAN      : "); Serial.println(!!(IFREGS.ctrl & MASK_CTRL_CLEAN_START));

  Serial.println("---------------------\n");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000); // safer with ESP32 slave

  Serial.println("Strike the Earth!");

  pinMode(BREWING_LED, OUTPUT);
  pinMode(BREWING_PAUSED_LED, OUTPUT);
  pinMode(GRINDING_LED, OUTPUT);
  pinMode(GRINDING_PAUSED_LED, OUTPUT);
  pinMode(CLEANING_LED, OUTPUT);
  pinMode(HEATER_LED, OUTPUT);
  pinMode(HEATER_PAUSED_LED, OUTPUT);

  digitalWrite(BREWING_LED, HIGH);
  delay(50);
  digitalWrite(BREWING_PAUSED_LED, HIGH);
  delay(50);
  digitalWrite(GRINDING_LED, HIGH);
  delay(50);
  digitalWrite(GRINDING_PAUSED_LED, HIGH);
  delay(50);
  digitalWrite(CLEANING_LED, HIGH);
  delay(50);
  digitalWrite(HEATER_LED, HIGH);
  delay(50);
  digitalWrite(HEATER_PAUSED_LED, HIGH);
  delay(50);

  reloadIFaceRegs();
  printIFaceRegs();

  digitalWrite(BREWING_LED, LOW);
  delay(50);
  digitalWrite(BREWING_PAUSED_LED, LOW);
  delay(50);
  digitalWrite(GRINDING_LED, LOW);
  delay(50);
  digitalWrite(GRINDING_PAUSED_LED, LOW);
  delay(50);
  digitalWrite(CLEANING_LED, LOW);
  delay(50);
  digitalWrite(HEATER_LED, LOW);
  delay(50);
  digitalWrite(HEATER_PAUSED_LED, LOW);
  delay(50);
}

void writeLedStates(int currentTemp, int currentWeight) {
  Serial.print("BREW START : "); Serial.println();
  Serial.print("BREW PAUSE : "); Serial.println();
  Serial.print("GRIND START: "); Serial.println();
  Serial.print("GRIND PAUSE: "); Serial.println();
  Serial.print("HEAT       : "); Serial.println();
  Serial.print("CLEAN      : "); Serial.println();

  int temp_reached = IFREGS.temp < currentTemp;
  int brew_force_paused = !!(IFREGS.ctrl & MASK_CTRL_BREW_PAUSE);
  
  int brew_start = !!(IFREGS.ctrl & MASK_CTRL_BREW_START);
  int brew_paused = brew_start && (temp_reached || brew_force_paused);

  int grind_start = !!(IFREGS.ctrl & MASK_CTRL_GRIND_START);
  int grind_paused = grind_start && (!!(IFREGS.ctrl & MASK_CTRL_GRIND_PAUSE));

  int cleaning = !!(IFREGS.ctrl & MASK_CTRL_CLEAN_START);

  int heater_on = !!(IFREGS.ctrl & MASK_CTRL_RESRV_HEAT);
  int heater_paused = heater_on && temp_reached;

  digitalWrite(BREWING_LED, brew_start);
  digitalWrite(BREWING_PAUSED_LED, brew_paused);

  digitalWrite(GRINDING_LED, grind_start);
  digitalWrite(GRINDING_PAUSED_LED, grind_paused);

  digitalWrite(CLEANING_LED, cleaning);

  digitalWrite(HEATER_LED, heater_on);
  digitalWrite(HEATER_PAUSED_LED, heater_paused);
}

// --------------------------------------------------

void loop() {
  reloadIFaceRegs();
  printIFaceRegs();

  int currentTemp = analogRead(A0);
  int currentWght = analogRead(A1);
  Serial.print("CURRENT TEMP: ");
  Serial.println(currentTemp);
  Serial.print("CURRENT WGHT: ");
  Serial.println(currentWght);
  delay(500);

  writeLedStates(currentTemp, currentWght);

}