#!/bin/bash

if [[ ! -f $1 ]] ; then { echo "$1 does not exist"; exit 1; } ; fi
if [[ ! -f $2 ]] ; then { echo "$2 does not exist"; exit 1; } ; fi

LOG1=$(mktemp FILE1.XXXXXXX)
LOG2=$(mktemp FILE2.XXXXXXX)

hist_file_dump.py $1 --hash > $LOG1
RV=$?
if [ $RV != 0 ]; then { echo "Failure dumping $1"; rm -f $LOG1 $LOG2; exit $RV ; }; fi
hist_file_dump.py $2 --hash > $LOG2
RV=$?
if [ $RV != 0 ]; then { echo "Failure dumping $2"; rm -f $LOG1 $LOG2; exit $RV ; }; fi

diff  $LOG1 $LOG2
RV=$?
if [ $RV != 0 ]; then
    echo "$1 <"
    echo "$2 >"
    echo "Files differ"
else
    echo "Files match"
fi

rm -f $LOG1 $LOG2
exit $RV
