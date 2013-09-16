#!/bin/sh -xv
#
# echo "Starting pre_check.sh"
#Cleaning
#echo "## cleaning"
test=$1
DBNAME=$2
/bin/rm -f ${test}.log
#
#echo "## setting up environment "
# source ../cmt/setup.sh
#echo "## ready to run"
#echo $CMTPATH
#echo $JOBOPTSEARCHPATH

# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4

# If DBNAME string is non-zero, then remove cool database
if [ -n "${DBNAME}" ]
   then
   echo "Remove cool database ${DBNAME}"
   coolDropDB "oracle://devdb10;schema=ATLAS_COOLTEST;user=ATLAS_COOLTEST;password=cool4devdb;dbname=${DBNAME}"
fi

# For test remove data file
#echo " For test remove data file"
#/bin/rm -f SimplePoolFile.root
