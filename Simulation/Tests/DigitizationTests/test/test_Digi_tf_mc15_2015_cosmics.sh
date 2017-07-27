#!/bin/sh
#
# art-description: Run digitization of a cosmics sample produced with MC15 using 2015 geometry and conditions
# art-type: grid


Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/testCosmics.ATLAS-R2-2015-03-01-00_VALIDATION.HITS.pool.root  --outputRDOFile mc15_2015_cosmics.RDO.pool.root  --maxEvents 100  --skipEvents 0  --digiSeedOffset1 11 --digiSeedOffset2 22  --geometryVersion ATLAS-R2-2015-03-01-00_VALIDATION  --conditionsTag default:OFLCOND-RUN12-SDR-25  --DataRunNumber 222500  --postInclude 'default:PyJobTransforms/UseFrontier.py'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME mc15_2015_cosmics.RDO.pool.root
