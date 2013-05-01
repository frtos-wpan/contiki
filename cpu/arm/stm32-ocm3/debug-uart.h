/**
 * USART driver for STM32xx w/ libopencm3 peripheral lib
 * (c) 2012 Blessed Contraptions
 * Jeff Ciesielski <jeff.ciesielski@gmail.com>
 */

#ifndef _DEBUG_UART_H_
#define _DEBUG_UART_H_

#include "debug-uart-arch.h"

int uart_putchar(char c);
void uart_init(int baud);
void uart_set_input(int (*input)(unsigned char c));

#endif