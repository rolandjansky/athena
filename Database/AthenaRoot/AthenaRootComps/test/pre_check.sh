#!/bin/sh -xv
#
echo "Starting pre_check.sh"
#Cleaning
#echo "## cleaning"
test=$1
/bin/rm -f ${test}.log
#
#echo "## setting up environment "
# source ../cmt/setup.sh
#echo "## ready to run"
#echo $CMTPATH
#echo $JOBOPTSEARCHPATH
