#!/bin/sh -xv
#
 echo "Starting pre_check.sh"
#Cleaning
#echo "## cleaning"
test=$1
/bin/rm -f ${test}.log

#add in lfns for pfns
FCregisterLFN -l MySimplePoolFile1 -p SimplePoolFile1.root
FCregisterLFN -l MySimplePoolFile2 -p SimplePoolFile2.root

#
echo "## setting up environment "
# source ../cmt/setup.sh
echo "## ready to run"
echo $CMTPATH
echo $JOBOPTSEARCHPATH

# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4

# For test remove data file
#echo " For test remove data file"
#/bin/rm -f SimplePoolFile.root
