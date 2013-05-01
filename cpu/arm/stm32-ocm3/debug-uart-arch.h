
#ifndef _DEBUG_UART_ARCH_H_
#define _DEBUG_UART_ARCH_H_

#include "contiki-conf.h"

#ifndef DEBUG_UART_CONF 
#error Must define DEBUG_UART_CONF (1,2,3)
#endif

#if (DEBUG_UART_CONF == 1)
#define DEBUG_UART USART1
#define DEBUG_UART_NVIC NVIC_USART1_IRQ
#define DEBUG_UART_ISR usart1_isr
#elif (DEBUG_UART_CONF == 2)
#define DEBUG_UART USART2
#define DEBUG_UART_NVIC NVIC_USART2_IRQ
#define DEBUG_UART_ISR usart2_isr
#elif (DEBUG_UART_CONF == 3)
#define DEBUG_UART USART3
#define DEBUG_UART_NVIC NVIC_USART3_IRQ
#define DEBUG_UART_ISR usart3_isr
#else
#error unknown DEBUG_UART_CONF definition!
#endif


/**
 * \brief ocm3 platforms often have different gpio setup
 * 
 * The underlying usart code is all common, but this needs to be implemented
 * to properly set up gpios.
 */
void uart_init_arch(void);

#endif