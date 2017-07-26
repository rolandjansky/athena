#!/bin/sh
#
# art-description: Tests ATLAS + ZDC simulation, generating events on-the-fly
# art-type: grid

AtlasG4_tf.py --preInclude 'G4AtlasTests/preInclude.ZDC_SingleParticle.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents 3 --randomSeed 10 --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --ZDCOn 'True' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.ZDC_dcube.py'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO This is a regression test I think. We would also need to compare these files to fixed references and run DCube
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME test.HITS.pool.root
