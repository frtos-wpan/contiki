/**
 * \file
 *         libopencm3 implementation of the contiki "clock" module
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

void sys_tick_handler(void) __attribute__((interrupt));

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

void
clock_init()
{
	systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);
	systick_set_reload(AHB_SPEED / 8 / CLOCK_SECOND);
	systick_interrupt_enable();
	systick_counter_enable();
}

clock_time_t
clock_time(void)
{
	return current_clock;
}

unsigned long
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
 * \return     void
 */
void
clock_delay(unsigned int t)
{
	clock_delay_usec(3 * t);
}

static
void inner_delay_usec_one(void)
{
	u32 before = STK_VAL;
	u32 diff = before - CLOCK_MICROSECOND_SYSTICK;
	if (diff > STK_LOAD) {
		// TODO -handle wrapping
	} else {
		while (STK_VAL > (diff)) {
			;
		}
	}
}

void
clock_delay_usec(uint16_t dt)
{
	while (dt--) {
		inner_delay_usec_one();
	}
}