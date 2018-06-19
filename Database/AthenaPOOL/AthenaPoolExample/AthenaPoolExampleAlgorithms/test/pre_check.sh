#!/bin/sh -xv
# Cleaning
test=${1}
/bin/rm -f ${test}.log ${test}.py
#
if [ "${test}" = "AthenaPoolExample_Write" ]
then
	/bin/rm -f *
elif [ "${test}" = "AthenaPoolExample_Concat" ]
then
	/bin/rm -f SimplePoolFile[13].root
elif [ "${test}" = "AthenaPoolExample_WMeta" ]
then
	/bin/rm -f *.root *.xml*
fi
# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4
