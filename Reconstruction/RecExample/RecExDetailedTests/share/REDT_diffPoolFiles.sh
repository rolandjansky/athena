#!/bin/bash

if [ $# -lt 2 ]
then
    echo "Not enough arguments !"
    exit -1
fi

echo reference file is $1
echo new file is $2

if [ "$1" = "/castor/cern.ch/some/fallback/file.root" ]
then
    echo "file1: got the fallback file, exiting"
    exit -2
fi
if [ "$2" = "/castor/cern.ch/some/fallback/file.root" ]
then
    echo "file2: got the fallback file, exiting"
    exit -2
fi

# first comparison, diffPoolFiles
echo "first step, running diffPoolFiles"
diffPoolFiles.py $1 $2 > diffPoolFiles.txt 2>&1 
RETCODE=$?
if [ $RETCODE -ne 0 ] 
then
    echo "problem: diffPoolFile detected difference between these files:"
    echo "ref: $1"
    echo "cmp: $2"
    exit -3
fi

echo "diffPoolFiles OK, proceeding to step 2, acmd.py dump-root"
# acmd.py dump-root --range 0:9 root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/rtt/rtt/rel_1/16.6.X/i686-slc5-gcc43-opt/offline/RecJobTransformTests/data_7TeV_allstreams/myESD.pool.root
# acmd.py dump-root --range 0:9 root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/rtt/rtt/rel_2/16.6.X/i686-slc5-gcc43-opt/offline/RecJobTransformTests/data_7TeV_allstreams/myESD.pool.root

TD=/tmp/$USER/adr.tmp.$EUID.$$
if [ ! -z "$TMPDIR" ]
then
    TD=$TMPDIR/adr.tmp.$EUID.$$
fi

mkdir -p $TD
echo "using as tempdir $TD"

Nev=`acmd.py chk-file $1 2>&1 | grep "Nbr Events:" | gawk '{print $3 }'`

echo reference file has $Nev events
RLAST=10
RFIRST=0
RANGE="$RFIRST:$RLAST"

while [ ! -z $RANGE ]
do
    echo testing range of events of $RANGE
    acmd.py dump-root --range $RANGE $1 | tee $TD/reference.log2 | gzip - > reference.range-$RFIRST-$RLAST.log.gz
    acmd.py dump-root --range $RANGE $2 > $TD/new.log
    
    FIDr=`grep -m1 "##Params.000.db_string " $TD/reference.log2 | sed "s#^.\+VALUE=\([0-9A-F,-]\+\).\+#\1#g"`
    FIDn=`grep -m1 "##Params.000.db_string " $TD/new.log | sed "s#^.\+VALUE=\([0-9A-F,-]\+\).\+#\1#g"`
    
    grep -v -e "\.m_token" -e "\.m_athenabarcode" -e "^Py:dump-root \+INFO fname:" -e "$FIDr" $TD/reference.log2 > $TD/reference.log
    grep -v -e "\.m_token" -e "\.m_athenabarcode" -e "^Py:dump-root \+INFO fname:" -e "$FIDn" $TD/new.log | diff --speed-large-files -s - $TD/reference.log > diff.dump-root.range-$RFIRST-$RLAST.log
    # zdiff -s $TD/reference.log.gz $TD/new.log.gz > diff.dump-root.range-$RFIRST-$RLAST.log
    RETCODE=$?
    
    rm -f $TD/*
    if [ $RETCODE -ne 0 ] 
    then
	echo "problem: difference in content of these poolfiles detected for events $RFIRST to $RLAST:"
	echo "ref: $1"
	echo "cmp: $2"
	grep "^>" diff.dump-root.range-$RFIRST-$RLAST.log | sed "s/^> //g" | gawk -F \. '{ print $1 " " $3 }' | sort | uniq -c | sort -nr
	# rm -fr $TD
	# exit -4
    fi
    # exit 0
    RANGE=""
    RLAST=$(( $RLAST + 10 ))
    RFIRST=$(( $RFIRST + 10 ))
    if [ $RLAST -lt $Nev ]
    then
	RANGE="$RFIRST:$RLAST"
    fi
done

rm -fr $TD

echo "all tests OK"
exit 0
