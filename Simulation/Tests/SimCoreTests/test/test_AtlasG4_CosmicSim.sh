#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF, generating events on-the-fly, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --outputHITSFile 'test.HITS.pool.root' --maxEvents '1500' --randomSeed '1234' --DataRunNumber '222525' --CosmicFilterVolume 'Calo' --CosmicFilterVolume2 'NONE' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --physicsList 'FTFP_BERT' --CosmicPtSlice 'NONE' --outputEVNT_TRFile 'test.TR.pool.root' --beamType 'cosmics' --postInclude 'PyJobTransforms/UseFrontier.py'

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
