#!/bin/sh

## Check arguments
if [ $# -lt 1 ] ; then
   echo "Usage: tct_dqflagscheck.sh <hanresultsfile> [<dqflagslist>] [<comparisonlist>]"
   exit 1 
fi

## set configuration parameters
hanresultsfile=$1
if [ ! -f $hanresultsfile ]; then
  echo "ERROR: Input han file <$hanresultsfile> not found. Exit."
  exit 1
fi

dqflagslist="dqflags.list"
if [ $# -gt 1 ] ; then
  dqflagslist=$2
fi

comparisonlist="None"
if [ $# -gt 2 ] ; then
  comparisonlist=$3
fi

## run command
RUNCMD="han-results-print.exe ${hanresultsfile}"
#han-results-print.exe run__han.root 2>&1 | tee dqflags.list; echo "ErrorCode=0 (OK)
$RUNCMD 2>&1 | tee $dqflagslist

## check overall status
status="Red"
if [ ! -f $comparisonlist ]; then
  status=`grep top_level ${dqflagslist} | gawk '{print $2}'`
  cp -f ${dqflagslist} summary.txt
else
  dqflagsdiff=`diff ${dqflagslist} ${comparisonlist}`
  echo ${dqflagsdiff} > summary.txt
  if [ "$dqflagsdiff" = "" ]; then
    status="Green"
  else  
    echo
    echo "ERROR: Difference in dq flag lists between today and yesterday."
    echo ${dqflagsdiff}
    echo
  fi
fi

echo
echo "Input han root file            : ${hanresultsfile}"
echo "Output Data Quality flags list : ${dqflagslist}"
echo "Comparison list                : ${comparisonlist}"
echo "Overall Data Quality status    : ${status}"
echo

if [ "$status" = "Green" ] ; then
  echo "TestCode=SUCCESS"
else
  echo "TestCode=FAILED"
fi
echo "ErrorCode=0 (OK)"

