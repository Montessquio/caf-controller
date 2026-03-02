#include "uart.h"
#include <em_device.h>

void init_uart(void) {
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO; // enable clock to GPIO peripheral
  (GPIO->P[0]).MODEL |=
      (GPIO_P_MODEL_MODE5_PUSHPULL // configure PA.5/PA.6 for TX/RX
       + GPIO_P_MODEL_MODE6_INPUT);
  (GPIO->USARTROUTE[1]).RXROUTE = (6 << 16);
  (GPIO->USARTROUTE[1]).TXROUTE = (5 << 16);
  (GPIO->USARTROUTE[1]).ROUTEEN =
      GPIO_USART_ROUTEEN_TXPEN + GPIO_USART_ROUTEEN_RXPEN;

  CMU->CLKEN0_SET = CMU_CLKEN0_USART1; // enable clock to UART1
  USART1->EN = USART_EN_EN;            // enable USART1 module
  USART1->CLKDIV = 0x27A << 3;         // set 115200 bd @ 38.4 MHz rate
  USART1->CMD = USART_CMD_RXEN + USART_CMD_TXEN; // enable TX, RX
  USART1->IEN_SET = USART_IF_RXDATAV; // enable RX data valid interrupt
}

void uart_putc(char c) {
  while (!(USART1->IF & USART_IF_TXBL)) {
  } // wait for ready bit to set
  USART1->TXDATA = c; // transmit byte c
}

void uart_puts(char *s) // write 0-terminated string
{
  int8_t i = 0;
  while (s[i] != 0) {
    uart_putc(s[i++]);
  }
}

void uart_putu(uint32_t d) // write 32-bit unsigned number
{
  uint8_t rem[8]; // storage for digits
  int32_t i = 0;
  if (d == 0) {
    uart_putc('0');
  } else {
    do
    {
      rem[i++] = d % 10;
      d = d / 10;
    } while (d != 0); // supress leading 0s
    for (i = i - 1; i >= 0; i--) // print digits
      uart_putc(rem[i] + '0');
  }
}

void uart_puti(int32_t d) // write 32-bit signed integer
{
  if (d < 0) {
    uart_putc('-');
    d = -d;
  }
  uart_putu((uint32_t)d);
}

char uart_getc(void) {
  while (!(USART1->IF & USART_IF_RXDATAV));
  return ((char)(USART1->RXDATA));
}

/*
uint8_t uart_gets(char *s, uint8_t len) // read a string from terminal
{                                        // s is the placeholder to store it
  uint8_t i = 0;                         // len is max length
  char c;                                // returns actual string length
  do {
    c = uart_getc();
    if (c == 13) {
      s[i] = 0; // terminating zero
      uart_putc('\r');
	  uart_putc('\n');
      break;
    }
    s[i++] = c;
  } while (i < len);
  return i;
}

int32_t uart_geti(void) // read a signed integer from terminal
{
  uint8_t i = 0;
  int32_t d = 0;
  char c, c1;
  c = uart_getc();
  c1 = c;
  while (i < 8) {
    if (c == 13) {
	  uart_putc('\r');
	  uart_putc('\n');
      break;
    }
    if ('0' <= c && c <= '9') {
      d = d * 10 + (c - '0');
      i++;
    }
    c = uart_getc();
  }
  if (c1 == '-')
    d = -d;
  return d;
}
*/
