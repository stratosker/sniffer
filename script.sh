#!/bin/bash
for tld do
	total=0
	
	times="$(grep ".$tld" *.out|cut -d' ' -f2)"

	for num in $times; do
		total=$((total + num))
	done
	echo $tld $total
done