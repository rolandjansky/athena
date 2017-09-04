#!/bin/sh
#
# art-description: Run simulation outside ISF, reading ttbar events, switching on extra timing checks, writing HITS, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/ttbar_muplusjets-pythia6-7000.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '10' --skipEvents '0' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:LArG4FastSimulation/LArG4FastSimulation_setupTimer_jobOptions.py'

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

# TODO Need to run DCube on timing histograms
#art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME test.HITS.pool.root
