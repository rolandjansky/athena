#!/bin/bash
DISK="CERN-PROD_SCRATCHDISK"
path="./runNumberFiles/"
for input in `find . -name '*_period_*.txt'`
do
    echo "This is the Inputfile: $input"

    for file in `cat $input`
    do
	dsid=`cut -d '.' -f2 <<< $file`
	outputFile=`echo "$input" | cut -d'.' -f1,2,3`"_DSID-"$dsid"_$DISK.txt"
	echo "This will be the outputfile : $outputFile"
	echo "Currently searching on SD for : $file"
#	rucio list-file-replicas $file| grep $DISK | cut -d ':' -f3,4,5 | cut -d '=' -f2
	rucio list-file-replicas $file | grep $DISK | cut -d ':' -f4,5 | cut -d '|' -f1 | cut -d '=' -f2 >> $path$outputFile
    done
#    sed -i -e 's/2811/root:\/\/eosatlas\//g' $path$outputFile
    sed -i -e 's/2811/root:\/\/eosatlas\//g' $path*
    sed -i -r 's/\s+//g' $path*
#    cat $path$outputFile

done