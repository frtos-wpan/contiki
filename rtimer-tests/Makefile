TESTS = one-timer one-cancel \
	resched \
	seq-idle

.PHONY:	test clean

test:
	for n in $(TESTS); do NAME=$$n ./$$n || exit; done

clean:
	rm -f _out _ref _tmp
	rm -f _rtimer _sim
