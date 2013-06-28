/*
 * Copyright (c) 2013, Karl Palsson <karlp@tweak.net.au>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \addtogroup stm32-ocm3
 * @{
 *
 * \defgroup stm32-ocm3-clock stm32-ocm3 Clock
 *
 * Implementation of the clock module for libopencm3 targets
 *
 * To implement the clock functionality, we use the SysTick peripheral.
 * @{
 *
 * \file
 * Clock driver implementation for libopencm3 targets
 */
#include <sys/clock.h>
#include <sys/cc.h>
#include <sys/energest.h>
#include <sys/etimer.h>

#include <libopencm3/cm3/systick.h>

static volatile clock_time_t current_clock = 0;
static volatile unsigned long current_seconds = 0;
static unsigned int second_countdown = CLOCK_SECOND;

/* Systick at AHB_SPEED/8 Mhz, this many ticks per usec */
#define CLOCK_MICROSECOND_SYSTICK ((AHB_SPEED/8)/1000000)

void
sys_tick_handler(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);
	current_clock++;
	if (etimer_pending() && etimer_next_expiration_time() <= current_clock) {
		etimer_request_poll();
	}

	if (--second_countdown == 0) {
		current_seconds++;
		second_countdown = CLOCK_SECOND;
	}
	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}


/**
 * \brief Arch-specific implementation of clock_init for libopencm3 target
 * 
 * The user specifies the AHB speed, and we run at div8.  For most cases, AHB
 * speed is equal to sysclock, but could still range from 8Mhz (Default HSI on
 * stm32f100 through to 168Mhz (stm32f4 at max speed)
 */
void
clock_init()
{
	systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);
	systick_set_reload(AHB_SPEED / 8 / CLOCK_SECOND);
	systick_interrupt_enable();
	systick_counter_enable();
}

CCIF clock_time_t
clock_time(void)
{
	return current_clock;
}

CCIF unsigned long
clock_seconds(void)
{
	return current_seconds;
}

void
clock_set_seconds(unsigned long sec)
{
	current_seconds = sec;
}

void
clock_wait(clock_time_t i)
{
	clock_time_t start;

	start = clock_time();
	while (clock_time() - start < i);
}

/**
 * \brief      Historically 2.83 uSecs, this implementation is in 3uSec ticks
 * \param t    As per clock.h
 * 
 * Obsolete, but included for completeness and compatibility
 */
void
clock_delay(unsigned int t)
{
	clock_delay_usec(3 * t);
}

static
void inner_delay_usec_one(void)
{
	uint32_t before = STK_VAL;
	while (STK_VAL - before <  CLOCK_MICROSECOND_SYSTICK) {
		;
	}
}

/**
 * \brief Arch-specific implementation for libopencm3 targets
 * \param len Delay \e dt uSecs
 *
 * Relies on the configured knowledge of how fast systick is running, rather
 * than using an entire timer for this.
 */
void
clock_delay_usec(uint16_t dt)
{
	while (dt--) {
		inner_delay_usec_one();
	}
}