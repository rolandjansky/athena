#!/bin/sh
#
# art-description: Tests detector response to single electrons, generated on-the-fly, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --preInclude 'SimulationJobOptions/preInclude.SingleElectronGenerator.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents '1000' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest.py' --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;'

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

# TODO Remake plots by reading in the HITS file produced by the previous job (Tests TP conversion) - need to run DCube on output
# athena G4AtlasTests/test_AtlasG4_electrons_noevgen.py


