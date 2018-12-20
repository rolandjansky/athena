#!/bin/sh -xv
#
# echo "Starting pre_check.sh"
#Cleaning
#echo "## cleaning"
test=$1
/bin/rm -f ${test}.log

# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4
