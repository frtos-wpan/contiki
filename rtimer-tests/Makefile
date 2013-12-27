TESTS = one-timer one-cancel \
	resched \
	seq-idle

.PHONY:	test cov clean

test:
	>_cov
	for n in $(TESTS); do NAME=$$n cov=_cov ./$$n || exit; done

cov:
	./merge.pl _cov

clean:
	rm -f _out _ref _tmp
	rm -f _rtimer _sim _cov
	rm -f sim.c.gcov sim.gcda sim.gcno _rtimer.gcov
