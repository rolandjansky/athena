#!/bin/sh
# art-type: build
# art-include: master/Athena

#

#!/bin/sh
# art-type: build
# art-include: master/Athena

#clear BS from previous runs
rm -rf  data_test.*.data
athena  --threads=1 --skipEvents=10 --evtMax=20 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" TrigUpgradeTest/egamma.withViews.py



FNAME=data_test.00327265.Single_Stream.daq.RAW._lb0100._Athena._0000.data 
if [ -f ${FNAME} ]
then
    athena --threads=1  --filesInput=${FNAME} -c "doL1Unpacking=False" TrigUpgradeTest/decodeBS.py &&
	checkxAOD.py myESDfromBS.pool.root
    
else
    echo "missing input BS file, preceeding test failed"
    exit -1
fi

