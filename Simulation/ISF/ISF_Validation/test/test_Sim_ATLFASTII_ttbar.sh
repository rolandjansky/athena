#!/bin/sh
#
# art-description: Run simulation  using ISF with the ATLFASTII simulator, reading ttbar events, writing HITS, using 2015 geometry and conditions
# art-type: grid

#FIXME need to copy input file to CVMFS
Sim_tf.py --conditionsTag 'default:OFLCOND-RUN12-SDR-19' --physicsList 'FTFP_BERT' --truthStrategy 'MC12' --simulator 'ATLFASTII' --postInclude 'default:PyJobTransforms/UseFrontier.py' 'G4AtlasTests/postInclude.DCubeTest.py' --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' --DataRunNumber '222525' --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' --inputEVNTFile "/afs/cern.ch/atlas/groups/Simulation/EVNT_files/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" --outputHITSFile "test.HITS.pool.root" --maxEvents 250

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
