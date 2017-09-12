#!/bin/sh
#
# art-description: Run test-beam simulation outside ISF, generating events on-the-fly, writing HITS
# art-type: grid

TestBeam_tf.py --outputHITSFile 'test.HITS.pool.root' --maxEvents '10' --Eta '0.35' --testBeamConfig 'tbtile' --postInclude 'PyJobTransforms/UseFrontier.py'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME test.HITS.pool.root
