#!/usr/bin/env bash
#
# This scripts removes some control characters from the log file
# to make them viewable in a browser
#

if [ $# -lt 1 ]; then
   echo "Syntax: `basename $0` [FILE] [FILE] ..."
   exit 1
fi

for log in $@; do   
   sed "s///g" $log > $log.tmp
   mv $log.tmp $log
done
#