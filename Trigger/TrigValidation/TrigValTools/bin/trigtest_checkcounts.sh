#!/bin/sh
# syntax: trigtest_checkcounts.C <tolerance> <reference file> <level: BOTH, EF, L2, HLT>
tol=$1
ref=$2 
if [ $# -eq 3 ]; then
   level=$3
else 
   level="BOTH"
fi
get_files -data TrigTest_CheckCounts.C
echo -e ".x TrigTest_CheckCounts.C($tol,\042${ref}\042,\042${level}\042) \n .q " | root -b >TrigTest_CheckCounts.log
# print the log file
cat TrigTest_CheckCounts.log
if [ `grep "outside tolerance" TrigTest_CheckCounts.log | wc -l `  -gt 0 ]; then
  echo 'count compare failed'
  exit 127 
fi 
exit 0
