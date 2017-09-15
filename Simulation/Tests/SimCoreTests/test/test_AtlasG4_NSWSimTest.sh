#!/bin/sh
#
# art-description: Tests NSW detector functionality using di-muon events, generating events on-the-fly
# art-type: grid

AtlasG4_tf.py --preInclude 'G4AtlasTests/ParticleGun_flatpt_2particle.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents '200' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-25' --physicsList 'FTFP_BERT' --DBRelease 'current' --postInclude 'G4AtlasTests/postInclude.DCubeTest.py,G4AtlasTests/postInclude.NSW.config.simu.py' --preExec 'simFlags.ReleaseGeoModel=False;simFlags.SimulateNewSmallWheel=True;'

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

