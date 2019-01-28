#!/bin/sh
# art-type: build
# art-include: master/Athena

#
FNAME=data_test.00327265.Single_Stream.daq.RAW._lb0100._Athena._0000.data 
if [ -f ${FNAME} ]
then
    athena --threads=1  --filesInput=${FNAME} -c "doL1Unpacking=False" TrigUpgradeTest/decodeBS.py &&
	checkxAOD.py myESDfromBS.pool.root
    
else
    echo "missing input BS file, preceeding test failed"
    exit -1
fi

