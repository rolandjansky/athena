#!/bin/bash

help() {

echo "Thank you for asking for help, "
echo "useage is:  ./MultiDQHistogramMerge.sh <dir>"
}

dir=$1

# Test args
if [ "$1" == "-h" ] || [ "$1" == '--help' ]
then
    help
    exit 0
fi

if [ $# -lt 1 ]
then
    echo "Too few arguments.  Usage is:"
    help
    exit 1
fi

echo "merging .root files in directory ${dir}"
echo "WARNING YOU SHOULD USE FULL PATH FOR THE DIRECTORY"

cd $dir

echo "making root file list in $dir"

#making the list of files to be merged
rm list.txt
touch list.txt
COUNTALL=0
for name in `ls $dir | grep root | grep -v out | grep -v merged | grep -v list`
do
    echo $((++COUNTALL))
    echo "write ${dir}/${name} into list"
    echo "${dir}/${name}" >> list.txt
done

echo "made list file"
DQHistogramMerge.py list.txt merged_monitoring-$COUNTALL.root True

echo "finished merging"


