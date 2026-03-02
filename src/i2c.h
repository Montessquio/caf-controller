#pragma once
#include <stdint.h>

void init_i2c(void);
void i2c_write(uint8_t *data, uint16_t length);
void i2c_read(uint8_t *buffer, uint16_t length);