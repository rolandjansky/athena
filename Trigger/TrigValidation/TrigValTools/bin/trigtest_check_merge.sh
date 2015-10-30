#!/bin/sh
# syntax: trigtest_checkmerge.C <tolerance> <reference file> <level: both, EF, L2>
file=$1 
reffile=$2
tol=$3


get_files -data TrigTest_CheckMerge.C
echo -e ".x TrigTest_CheckMerge.C($tol,\042${file}\042) \n .q " | root -b > TrigTest_CheckMerge.log

check_merge_result=0
# print the log file
cat TrigTest_CheckMerge.log
if [ `grep "ERRORS found" TrigTest_CheckMerge.log | wc -l`  -gt 0 ]; then
  echo 'check merge failed'
  check_merge_result=127 
fi 

if [ `grep "WARNING" TrigTest_CheckMerge.log | wc -l`  -gt 0 ]; then
  echo 'TrigTest_CheckMerge test has WARNINGss'
  check_merge_result=1 
fi 

if [ $check_merge_result -eq 0 ]; then
    echo "check merge success"
fi

echo "Now running custom rootcomp"
get_files -jo rootcomp.py
rootcomp.py -S -e "TrigSteer_HLT/TrigROBMoni|ROBDataProviderSvc" $file $reffile

exit $check_merge_result
