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
	/bin/rm -f Catalog1.xml* SimplePool*1.root SimplePool*2.root SimplePool*3.root SimplePool*4.root
	/bin/rm -f *.root *.xml* *.h5
elif [ "${test}" == "AthenaPoolExample_RWcBs" ]
then
	cat ../share/AthenaPoolExample_RWBsJobOptions.py | \
	sed -e 's/^#svcMgr.ByteStreamInputSvc.FullFileName/svcMgr.ByteStreamInputSvc.FullFileName/g' > ${test}.py
	AtlCopyBSEvent.exe -d -e 14350,14356,14382 -o test_defl.data /afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data
fi
# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4
