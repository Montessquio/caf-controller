#include "efr32bg22c224f512im40.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "system_efr32bg22.h"
#include <stdint.h>

#include "common.h"
#include "i2c.h"
#include "uart.h"

#define ESP32_ADDR 0x42

void init_i2c(void) {
  CMU->CLKEN0_SET = 1 << 26; // GPIO
  CMU->CLKEN0_SET = 1 << 14; // I2C0

  // Configure PD.2,3 for open drain
  GPIO->P_SET[gpioPortD].MODEL = 0x88 << 8;

  // Route I2C pins
  GPIO->I2CROUTE->SCLROUTE = (3 << 16) + 3;
  GPIO->I2CROUTE->SDAROUTE = (2 << 16) + 3;
  GPIO->I2CROUTE->ROUTEEN = 3;

  I2C0->CLKDIV = 4;
  I2C0->CTRL = 1 << 4;
  I2C0->EN = 1;
}

void i2c_write(uint8_t *data, uint16_t len) {
  uint32_t i;

  // Send START
  I2C0->IF_CLR = 0xFFFFF;
  I2C0->CMD = (1 << 5) | (1 << 0);
  while (!(I2C0->IF & I2C_IF_START))
    ;
  I2C0->IF_CLR = I2C_IF_START;

  // Send address
  I2C0->TXDATA = ESP32_ADDR << 1;

  // Wait for the transmit buffer to be empty.
  while (!(I2C0->IF & (1 << 11)))
    ;

  // Check for error from the slave
  if (I2C0->IF & I2C_IF_NACK) {
    I2C0->IF_CLR = I2C_IF_NACK;
    I2C0->CMD = I2C_CMD_STOP;
    uart_puts("Address NACK\r\n");
    return;
  }

  // Send data bytes one by one
  for (i = 0; i < len; i++) {
    I2C0->TXDATA = data[i];

    // Small delay to ensure last byte fully clocked out
    delay(1000)

    // Wait for TXBL to indicate we can send another byte
    while (!(I2C0->IF & (1 << 11))) // TXBL
      ;

    // Also wait for ACK flag
    while (!(I2C0->IF & (1 << 6)))
      ;

    // Check for error from the slave
    if (I2C0->IF & I2C_IF_NACK) {
      I2C0->IF_CLR = I2C_IF_NACK;
      I2C0->CMD = (1 << 4);
      uart_puts("Data NACK\r\n");
      break;
    }
  }

  // Small delay to ensure last byte fully clocked out
  delay(1000)

  // Send STOP
  I2C0->CMD = I2C_CMD_STOP;
  while (!(I2C0->IF & I2C_IF_MSTOP))
    ;
  I2C0->IF_CLR = I2C_IF_MSTOP;
}

void i2c_read(uint8_t *buffer, uint16_t len) {
  uint32_t i;

  if (len == 0) {
    return;
  }

  // Send START
  I2C0->IF_CLR = 0xFFFFF;
  I2C0->CMD = (1 << 5) | (1 << 0);
  while (!(I2C0->IF & I2C_IF_START))
    ;
  I2C0->IF_CLR = I2C_IF_START;

  // Send address + READ bit
  I2C0->TXDATA = (ESP32_ADDR << 1) | 1;

  // Wait for something to be ready.
  while (!(I2C0->IF & I2C_IF_TXC))
    ;

  // Check for error from the slave
  if (I2C0->IF & I2C_IF_NACK) {
    I2C0->IF_CLR = I2C_IF_NACK;
    I2C0->CMD = I2C_CMD_STOP;
    uart_puts("Address NACK\r\n");
    return;
  }

  // Clear waiting flag.
  I2C0->IF_CLR = I2C_IF_TXC;

  // Read bytes
  for (i = 0; i < len; i++) {
    if (i == (len - 1)) {
      // Send a NACK to tell the slave
      // we can't read any more.
      I2C0->CMD = I2C_CMD_NACK;
    } else {
      // Send an ACK to tell the slave
      // that we want more bytes.
      I2C0->CMD = I2C_CMD_ACK;
    }

    // Wait for the next byte with timeout
    uint32_t timeout = 100000;
    while (!(I2C0->IF & I2C_IF_RXDATAV) && --timeout)
      ;

    if (timeout == 0) {
      uart_puts("RXDATAV timeout!\r\n");
      break;
    }

    // Save the read byte
    buffer[i] = (uint8_t)I2C0->RXDATA;

    // Clear waiting flag
    I2C0->IF_CLR = I2C_IF_RXDATAV;

    // Small delay to make sure the slave can keep up.
    delay(500);
  }

  // Ensure everything is flushed and clear
  delay(1000);

  // Send STOP
  I2C0->CMD = I2C_CMD_STOP;
  while (!(I2C0->IF & I2C_IF_MSTOP))
    ;
  I2C0->IF_CLR = I2C_IF_MSTOP;
}