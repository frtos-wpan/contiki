#!/usr/bin/perl
$pos = 0;
while (<>) {
	print;
	next if /^\s*$/;
	next if /^#/;
	if (/^[a-z]/ && !$scan) {
		$arm = 1;
		$scan = 0;
		next;
	}
	if (/^\s+[a-z]/) {
		next unless $scan;
	}
	if (/^{/) {
		$scan = $arm;
		$arm = 0;
	}
	if (/^}/) {
		$scan = 0;
		next;
	}
	next unless $scan;
	next if /^\s*return\s/;
	print "probe($pos);\n";
	$pos++;
}
printf "\n#define PROBE_POSITIONS $pos\n";
