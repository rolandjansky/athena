#!/bin/bash
# 

echo "==================Checking for FPEs"
grep --quiet "WARNING FPE" log.*
fpeStat=$?
# Let's flip this - finding a FPE is a failure and it'd be confusing to check for 0
if [[ "$fpeStat" == "0" ]]; then
  fpeStat="1"
else
  fpeStat="0"
fi

if [[ "$fpeStat" != "0" ]]; then
  echo "Found FPEs! FAILING the test. FPEs reported below." `date`
  for file in `ls log.*`;
    do
      echo "=====" $file;
      grep "WARNING FPE" $file | awk -F] '{print $2}' | sed 's/^.*\[//' | sort  | uniq -c
    done
fi

exit $fpeStat
