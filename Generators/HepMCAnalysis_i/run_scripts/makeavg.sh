#!/bin/bash

#Script to automatically perform the average of root histograms for the reference plots

if [[ -z $1 ]]
    then
    echo "usage: ./makeavg.sh <process>"
    echo "available processes:"
    ls out -1
    exit -1
fi

STARTDIR=`pwd`

PROCESS=$1
DIR=out/$PROCESS

if [[ ! -e $DIR ]]
    then
    echo "process $PROCESS not found in out directory"
    echo "available processes:"
    ls out -1
    exit -1
fi

echo "Average histograms in directory $DIR" 

cd $DIR

echo "found `ls root/*.root | wc -l` files"

cd root
root -q -b -l ../../../share/make_avg.C+
mkdir $STARTDIR/reference >& /dev/null
mv avg_histos.root $STARTDIR/reference/${PROCESS}.root
