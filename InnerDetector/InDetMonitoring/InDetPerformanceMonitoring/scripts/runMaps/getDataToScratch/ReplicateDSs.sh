#!/bin/bash
site="CERN-PROD_SCRATCHDISK"
path="./"
for input in `find . -name '*_period_*.txt'`
do
    echo "This is the Inputfile: $input"
    for file in `cat $input`
    do
	echo "Currently replicating : $file to $site"
	rucio add-rule $file --grouping DATASET 1 "$site"
    done

done