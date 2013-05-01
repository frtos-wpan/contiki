/**
 * \file
 *         Uart driver code for locm3 based targets.
 * 
 * Relies on an arch file to setup _which_ uart and configure gpios
 * appropriately.
 */
#include <errno.h>
#include <stdio.h>

#include <sys/energest.h>
#include <lib/ringbuf.h>

#include "contiki-conf.h"

#include "debug-uart.h"

#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

static struct ringbuf ringbuf_tx;
static uint8_t buf_tx[128];

static int (*uart_input_handler)(unsigned char c);

void uart_init(int baud)
{
	ringbuf_init(&ringbuf_tx, buf_tx, sizeof (buf_tx));
	uart_init_arch();

	nvic_enable_irq(DEBUG_UART_NVIC);
	nvic_set_priority(DEBUG_UART_NVIC, 2);

	usart_set_baudrate(DEBUG_UART, baud);
	usart_set_databits(DEBUG_UART, 8);
	usart_set_parity(DEBUG_UART, USART_PARITY_NONE);
	usart_set_flow_control(DEBUG_UART, USART_FLOWCONTROL_NONE);
	usart_set_stopbits(DEBUG_UART, USART_CR2_STOPBITS_1);
	usart_set_mode(DEBUG_UART, USART_MODE_TX_RX);
	usart_enable_rx_interrupt(DEBUG_UART);
	usart_enable(DEBUG_UART);

	/* This ensures stdio doesn't use its own buffers */
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
}

void
uart_set_input(int (*input)(unsigned char c))
{
	uart_input_handler = input;
}

/**
 *  \brief      Pushes a char into the output buffer
 * This is blocking.  It used to return -ENOMEM, which is very nice for posix,
 * but no-one was ever looking at it, so we simply truncated lots of writes
 */
int
uart_putchar(char c)
{
	if (c == '\n')
		uart_putchar('\r');
	/* try, try try again! */
	while (ringbuf_put(&ringbuf_tx, c) == 0);
	usart_enable_tx_interrupt(DEBUG_UART);
	return 0;
}

void
DEBUG_UART_ISR(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);
	char c;
	if (usart_get_flag(DEBUG_UART, USART_SR_TXE)) {
		if (ringbuf_elements(&ringbuf_tx) > 0) {
			c = ringbuf_get(&ringbuf_tx);
			usart_send(DEBUG_UART, (uint16_t) c);
		} else {
			usart_disable_tx_interrupt(DEBUG_UART);
		}
	}

	if (usart_get_flag(DEBUG_UART, USART_SR_RXNE)) {
		c = usart_recv(DEBUG_UART);
		if (uart_input_handler != NULL) {
			if (uart_input_handler(c)) {
				// Need to wakeup from low power mode here
			}
		}
	}
	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
