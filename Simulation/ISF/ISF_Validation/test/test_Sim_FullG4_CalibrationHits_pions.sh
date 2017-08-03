#!/bin/sh
#
# art-description: Run simulation using ISF with the FullG4 simulator, reading single pion events, writing HITS including full CaloCalibrationHit information, using 2015 geometry and conditions
# art-type: grid

Sim_tf.py --conditionsTag 'default:OFLCOND-RUN12-SDR-19' --physicsList 'FTFP_BERT' --truthStrategy 'MC12' --simulator 'FullG4'  --postInclude 'G4AtlasTests/postInclude.DCubeTest_CaloCalibHits.py' --preExec 'simFlags.ReleaseGeoModel=False;' --preInclude 'SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py' --DataRunNumber '222525' --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/pi_E50_eta0-60.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '10'

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
