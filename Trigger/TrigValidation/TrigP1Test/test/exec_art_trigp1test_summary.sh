#!/bin/bash

echo "====================================="
echo $(date "+%FT%H:%M %Z")"     Test Summary:"
if [ -f ${JOB_LOG} ]; then
   grep "art-result:" ${JOB_LOG} | awk '{split($0,a,":"); print a[2] }' > summary.log
   cat summary.log
fi
echo "====================================="
