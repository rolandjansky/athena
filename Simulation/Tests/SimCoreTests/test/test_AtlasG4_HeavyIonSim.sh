#!/bin/sh
#
# art-description: Run simulation outside ISF, reading lead ions peripheral simulation (low multiplicity) events, writing HITS, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/pbpb_Peripheral-hijing-5500.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '1' --skipEvents '4' --randomSeed '10' --geometryVersion 'ATLAS-R1-2012-03-00-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '210184' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:SimulationJobOptions/postInclude.HijingPars.py'

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
