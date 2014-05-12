#!/bin/sh
# syntax: trigtest_checkmerge.C <tolerance> <reference file> <level: both, EF, L2>
tol=$1
ref=$2 
if [ $# -eq 3 ]; then
   level=$3
else 
   level="BOTH"
fi

get_files -data TrigTest_CheckMerge.C
#ls -alg expert-monitoring.root
echo -e ".x TrigTest_CheckMerge.C($tol,\042${ref}\042) \n .q " | root -b > TrigTest_CheckMerge.log

# print the log file
cat TrigTest_CheckMerge.log
if [ `grep "ERRORS found" TrigTest_CheckMerge.log | wc -l`  -gt 0 ]; then
  echo 'check merge failed'
  exit 127 
fi 
echo "check merge success"
exit 0
