#!/bin/sh -xv
#
# echo "Starting pre_check.sh"
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

# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4

# For test remove data file
#echo " For test remove data file"
#/bin/rm -f SimplePoolFile.root
