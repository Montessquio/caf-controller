#include "efr32bg22c224f512im40.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "system_efr32bg22.h"
#include <stdint.h>

#include "common.h"
#include "uart.h"
#include "gpio.h"
#include "i2c.h"

uint8_t* msg = (uint8_t*)"MSG";

__NO_RETURN void _start(void) {
  init_uart();
  init_gpio();
  init_i2c();

  uart_puts("Strike the Earth!\r\n");
  while (1) {
    uart_puts(">> ");
    uart_puts((char*)msg);
    uart_puts("\r\n");

    i2c_write(msg, 4);
  
    delay(1000000);
    
    uint8_t buf[4];
    
    i2c_read((uint8_t*)&buf, 4);
    uart_puts("<< ");
    uart_puts((char*)buf);
    uart_puts("\r\n");

    delay(1000000);
  }
}
