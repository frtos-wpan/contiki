#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define	PRINTF(...)


/* ----- Helper functions for simulations ---------------------------------- */

 
static struct rtimer ra, rb, rc;
static char pa, pb, pc;
static char px = 'x', py = 'y', pz = 'z';


#define	__R(x, r, t, p)				\
	{ printf("S(" x ",%u", t);		\
	  rtimer_set(r, t, 0, handler, p);	\
	  printf(")"); }
#define	RA(t)	__R("A", &ra, t, &pa)
#define	RB(t)	__R("B", &rb, t, &pb)
#define	RC(t)	__R("C", &rc, t, &pc)

#define	__Rx(x, r, t, p)				\
	{ printf("S%c(" x ",%u", *p, t);		\
	  rtimer_set(r, t, 0, handler, p);	\
	  printf(")"); }
#define	RAx(t)	__Rx("A", &ra, t, &px)
#define	RAy(t)	__Rx("A", &ra, t, &py)
#define	RAz(t)	__Rx("A", &ra, t, &pz)
#define	RBx(t)	__Rx("B", &rb, t, &px)
#define	RBy(t)	__Rx("B", &rb, t, &py)
#define	RBz(t)	__Rx("B", &rb, t, &pz)
#define	RCx(t)	__Rx("C", &rc, t, &px)
#define	RCy(t)	__Rx("C", &rc, t, &py)
#define	RCz(t)	__Rx("C", &rc, t, &pz)

#define	__C(x, r)		\
	{ printf("C(" x);	\
	  rtimer_cancel(r);	\
	  printf(")"); }
#define	CA	__C("A", &ra)
#define	CB	__C("A", &rb)
#define	CC	__C("A", &rc)


/* ----- Inclusion of the instrumented rtimer code ------------------------- */


unsigned short rtimer_arch_now(void);

static void probe(unsigned here);
static void advance(unsigned t);


#include "_rtimer"


VARS


/* ----- Simulation timekeeping -------------------------------------------- */


static rtimer_clock_t now, next;
static bool running = 0;


void rtimer_arch_init(void)
{
	running = 0;
	now = next = 0;
}


rtimer_clock_t rtimer_arch_now(void)
{
	return now;
}


void rtimer_arch_schedule(rtimer_clock_t t)
{
	next = t > now ? t : now;
	running = 1;
}


/* ----- Timer handler ----------------------------------------------------- */


static char id(void *x, void *a, void *b, void *c)
{
	if (x == a)
		return 'A';
	if (x == b)
		return 'B';
	if (x == c)
		return 'C';
	return '?';
}


static void handler(struct rtimer *rt, void *ptr)
{
	char rx = id(rt, &ra, &rb, &rc);
	char p = id(ptr, &px, &py, &pz);

	if (p == '?') {
		p = id(ptr, &pa, &pb, &pc);
		if (rx != p) {
			fprintf(stderr, "R%c with ptr %c\n", rx, p);
			exit(1);
		}
		printf("T(%c,%u", rx, now);
	} else {
		printf("T%c(%c,%u", *(char *) ptr, rx, now);
	}
	{ HANDLER_ACTION }
	printf(")");
}


/* ----- Probe handler ----------------------------------------------------- */


static int pos;
static int count;


static void probe(unsigned here)
{
	if (here != pos)
		return;
	if (count--)
		return;
//	printf("P");
	{ PROBE_ACTION }
}


/* ----- Simulation -------------------------------------------------------- */


static void reset_sim(void)
{
	next_rtimer = NULL;
	locked = 0;
	deferred = 0;
	set_queue = NULL;
	setting = 0;

	memset(&ra, 0, sizeof(ra));
	memset(&rb, 0, sizeof(rb));
	memset(&rc, 0, sizeof(rc));

	rtimer_init();
}


static void run_sim(void)
{
	reset_sim();

	{ INIT_ACTION }

	while (running) {
		if (now < next)
			now = next;
		running = 0;
		rtimer_run_next();
	}
}


static void __attribute__((unused)) advance(unsigned t)
{
	if (t < now) {
		fprintf(stderr, "going back in time (%u < %u)\n", t, now);
		exit(1);
	}
	now = t;
	if (running && next <= now) {
		running = 0;
		rtimer_run_next();
	}
}


/* ----- Main -------------------------------------------------------------- */


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	int c;

	if (argc != 1)
		usage(*argv);

	/*
	 * We skip probe 0 because that would be in rtimer_init right before
	 * calling rtimer_arch_init. This is an invalid usage scenario that
	 * produces weird results that may mask real issues.
	 */
	for (pos = 1; pos < PROBE_POSITIONS; pos++) {
		c = 0;
		while (1) {
			count = c;
			run_sim();
			printf(" # %u %d\n", pos, c);
			if (count >= 0)
				break;
			c++;
		}
	}

	return 0;
}
