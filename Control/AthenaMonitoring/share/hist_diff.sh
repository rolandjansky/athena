#!/bin/bash
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

FILE1=$1
FILE2=$2
shift 2

GREPOPTS="--regexp=.*"
DUMPOPTS=""
while getopts "itx:" opt; do
    case ${opt} in
	x )
	    echo "Will ignore: $OPTARG"
	    GREPOPTS="-v -E $OPTARG"
	    ;;
	i )
		echo "Doing fuzzy check"
		DUMPOPTS="$DUMPOPTS --fuzzy_histbins --no_onfile -r name"
		;;
	t )
		echo "Doing loose TTree check"
		DUMPOPTS="$DUMPOPTS --tree_entries"
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

echo "Invoking" $(which hist_file_dump.py)
hist_file_dump.py $FILE1 --hash ${DUMPOPTS} | grep ${GREPOPTS} > $LOG1
RV=$?
if [ $RV != 0 ]; then { echo "Failure dumping $FILE1"; echo $RV ; cat $LOG1; rm -f $LOG1 $LOG2; exit $RV ; }; fi
hist_file_dump.py $FILE2 --hash ${DUMPOPTS} | grep ${GREPOPTS} > $LOG2
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
