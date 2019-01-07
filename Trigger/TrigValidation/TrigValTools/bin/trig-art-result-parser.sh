#!/bin/bash

#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

# This script is used to parse output (stdout) from locally-ran Trigger ART tests in order to define the final return
# code of the test. Similarly to ART, it looks for lines starting with "art-result:" and checks the corresponding codes.
# The script returs zero if all results are zero. Otherwise, it returns the first non-zero code it finds.
#
# Usage:
# trig-art-result-parser.sh logfile [stepsToCheck]
#
# Example:
# test_physics_pp_v7_build.sh > stdout.txt
# retcode=`trig-art-result-parser.sh stdout.txt "athena CheckLog RegTest"`
# echo "test_physics_pp_v7_build final result is $retcode"

# The log file to parse
logfile=$1

# Step names to check (matching is done to the beginning of the name)
stepsToCheck=($2)

# If stepsToCheck not provided, set the default value
if [ -z "$stepsToCheck" ]; then
  stepsToCheck=(athena CheckLog)
fi

# Take only the lines starting from 'art-result:'
results=`grep '^art-result:' $logfile`

# Iterate over the result lines and parse each of them
echo "========== art-result summary =========="
retcode=0
IFS=$'\n' # make newlines the only separator
for result in $results; do
  resultcode=`echo $result | cut -d ' ' -f 2`
  stepname=`echo $result | cut -d ' ' -f 3`

  # Ignore steps which we don't want to check
  ignoreThisStep=1
  for token in "${stepsToCheck[@]}"; do
    if [[ "$stepname" == "$token"* ]]; then
      ignoreThisStep=0
      break
    fi
  done
  if [ "$ignoreThisStep" -eq 1 ]; then
    continue
  fi

  # Print the result and check the return code
  echo $result
  if [ "$resultcode" -ne "0" ] && [ "$retcode" -eq "0" ]; then
    retcode=$resultcode
    echo "ERROR: non-zero return code detected, will return $retcode as the final result"
  fi
done
unset IFS

exit $retcode
