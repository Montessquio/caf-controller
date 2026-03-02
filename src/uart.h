#pragma once
#include <stdint.h>

void init_uart(void);
void uart_putc(char);
void uart_puts(char*);
void uart_putu(uint32_t);
void uart_puti(int32_t);
char uart_getc(void);
uint8_t uart_gets(char*, uint8_t);
int32_t uart_geti(void);
