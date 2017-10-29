#!/bin/sh
#
# art-description: Run digitization on a fixed HITS file
# art-type: grid

Digi_tf.py --inputHITSFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._0004*.pool.root*' --digiSeedOffset1 '1' --digiSeedOffset2 '2' --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-28' --DataRunNumber '222525' --maxEvents '50' --outputRDOFile 'MCtest_ttbar.digit.pool.root' --imf False
echo "art-result: $?"

Reco_tf.py --outputTAGFile=myTAG_ttbar_2.root --outputHISTFile=myMergedMonitoring_ttbar_2.root --maxEvents=500 --outputESDFile=myESD_ttbar_2.pool.root --outputAODFile=myAOD_ttbar_2.AOD.pool.root --inputRDOFile=MCtest_ttbar.digit.pool.root --autoConfiguration=everything --triggerConfig=NONE --preExec 'rec.doTrigger=False;'
echo "art-result: $?"

SCRIPT_DIRECTORY=$1 
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_ttbar_2.AOD.pool.root myESD_ttbar_2.pool.root
echo "art-result: $?"

art.py compare grid --days=3 $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_ttbar_2.AOD.pool.root myESD_ttbar_2.pool.root
echo "art-result: $?"
