#!/bin/bash

grep -v IsActiveStatus $1 | grep -v "WILL SOON NOT WORK ANYMORE" >& stdout_cleaned.txt

export GREP_ERROR=0
#grep returns 1 if nothing is found, whilst ART assumes a non-zero code indicates a problem. So need
#to revers the error code if 0 is returned by grep (by default assume error code zero in initialization above)
grep ERROR stdout_cleaned.txt
if [ $? == 0 ]; then
    export GREP_ERROR=1
fi    
echo "art-result: $GREP_ERROR grepping log file for errors."

