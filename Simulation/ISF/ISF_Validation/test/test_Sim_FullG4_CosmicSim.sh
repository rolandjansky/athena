#!/bin/sh
#
# art-description: Run cosmics simulation using ISF with the FullG4 simulator, generating events on-the-fly, using 2015 geometry and conditions
# art-type: grid

Sim_tf.py --conditionsTag 'OFLCOND-RUN12-SDR-19' --physicsList 'QGSP_BERT' --truthStrategy 'MC12' --simulator 'FullG4' --outputEVNT_COSMICSTRFile 'test.TR.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '1500' --randomSeed '1234' --DataRunNumber '10' --CosmicFilterVolume 'Calo' --CosmicFilterVolume2 'NONE' --preInclude 'SimulationJobOptions/preInclude.Cosmics.py' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --CosmicPtSlice 'NONE' --beamType 'cosmics'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME test.HITS.pool.root test.TR.pool.root
