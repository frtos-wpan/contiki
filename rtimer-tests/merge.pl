#!/usr/bin/perl
while (<>) {
	die unless /^\s*(-|#####|\d+):\s*(\d+):/;
	$n = $2;
	next unless $n;
	if (defined $line{$n}) {
		die if $line{$n} ne $';
	} else {
		$line{$n} = $';
	}
	if ($1 eq "-") {
		if (defined $n{$n}) {
			die if $n{$n} ne "-";
		} else {
			$n{$n} = "-";
		}
		next;
	}
	die if $n{$n} eq "-";
	next if $1 eq "#####";
	$n{$n} += $1;
}

for (sort { $a <=> $b } keys %line) {
	if ($n{$_} eq "-") {
		print "    -"
	} elsif ($n{$_}) {
		printf("%5d", $n{$_});
	} else {
		print "#####";
	}
	printf(":%5d:%s", $_, $line{$_});
}
