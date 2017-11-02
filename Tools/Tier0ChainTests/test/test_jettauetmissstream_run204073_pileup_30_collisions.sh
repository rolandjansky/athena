#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_JetTauEtmiss_0.root --AMI=q205 --maxEvents=300 --outputESDFile=myESD_JetTauEtmiss_0.pool.root --geometryVersion=ATLAS-R1-2012-03-00-00 --outputAODFile=myAOD_JetTauEtmiss_0.AOD.pool.root --outputTAGFile=myTAG_JetTauEtmiss_0.root --ignoreErrors=False --conditionsTag=COMCOND-BLKPA-RUN1-07 --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data12_8TeV.00204073.physics_JetTauEtmiss.merge.RAW._lb0144._SFO-5._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data12_8TeV.00204073.physics_JetTauEtmiss.merge.RAW._lb0145._SFO-7._0001.1 --autoConfiguration=everything --preExec='rec.doTrigger=False;' --imf False
echo "art-result: $?"

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_JetTauEtmiss_0.AOD.pool.root myESD_JetTauEtmiss_0.pool.root
echo "art-result: $?"

