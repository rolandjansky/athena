#!/bin/bash

FILE1=$1
FILE2=$2
shift 2

GREPOPTS="--regexp=.*"
while getopts "x:" opt; do
    case ${opt} in
	x )
	    echo "Will ignore: $OPTARG"
	    GREPOPTS="-v -E $OPTARG"
	    ;;
	\? )
	    echo "Invalid option: -$OPTARG"
	    exit 1
	    ;;
    esac
done

if [[ ! -f $FILE1 ]] ; then { echo "$FILE1 does not exist"; exit 1; } ; fi
if [[ ! -f $FILE2 ]] ; then { echo "$FILE2 does not exist"; exit 1; } ; fi

LOG1=$(mktemp F1.XXXXXXX)
LOG2=$(mktemp F2.XXXXXXX)

hist_file_dump.py $FILE1 --hash | grep ${GREPOPTS} > $LOG1
RV=$?
if [ $RV != 0 ]; then { echo "Failure dumping $FILE1"; echo $RV ; cat $LOG1; rm -f $LOG1 $LOG2; exit $RV ; }; fi
hist_file_dump.py $FILE2 --hash | grep ${GREPOPTS} > $LOG2
RV=$?
if [ $RV != 0 ]; then { echo "Failure dumping $FILE2"; rm -f $LOG1 $LOG2; exit $RV ; }; fi

diff  $LOG1 $LOG2
RV=$?
if [ $RV != 0 ]; then
    echo "$FILE1 <"
    echo "$FILE2 >"
    echo "Files differ"
else
    echo "Files match"
fi

rm -f $LOG1 $LOG2
exit $RV
