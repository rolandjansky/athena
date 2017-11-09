#!/bin/sh
#
# art-description: Run digitization of an MC15 ttbar sample with 2012 geometry and conditions, without pile-up
# art-type: grid


Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ttbar.ATLAS-R1-2012-03-00-00.HITS.pool.root --outputRDOFile mc15_2012_ttbar.RDO.pool.root --maxEvents 25 --skipEvents 0  --digiSeedOffset1=11 --digiSeedOffset2=22 --geometryVersion ATLAS-R1-2012-03-00-00 --conditionsTag OFLCOND-RUN12-SDR-22 --DataRunNumber 212272 --postInclude 'default:PyJobTransforms/UseFrontier.py'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME mc15_2012_ttbar.RDO.pool.root
