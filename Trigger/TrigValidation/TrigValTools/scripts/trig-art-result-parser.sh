#!/bin/bash

logfile=$1
results=`grep '^art-result:' $logfile`

echo "========== art-result summary =========="
retcode=0
IFS=$'\n' # make newlines the only separator
for result in $results; do
  resultcode=`echo $result | cut -d ' ' -f 2`
  stepname=`echo $result | cut -d ' ' -f 3`

  # ignore steps which are not athena or CheckLog
  if [[ "$stepname" != "athena"* ]] && [[ "$stepname" != "CheckLog"* ]]; then
    continue
  fi

  # print the result and check the return code
  echo $result
  if [ "$resultcode" -ne "0" ] && [ "$retcode" -eq "0" ]; then
    retcode=$resultcode
    echo "ERROR: non-zero return code detected, will return $retcode as the final result"
  fi
done
unset IFS

exit $retcode
