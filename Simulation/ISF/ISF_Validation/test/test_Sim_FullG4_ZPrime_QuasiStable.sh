#!/bin/sh
#
# art-description: Run simulation using ISF with the FullG4_LongLived simulator, reading events, writing HITS, using 2015 geometry and conditions
# art-type: grid

Sim_tf.py --conditionsTag 'default:OFLCOND-RUN12-SDR-19' --physicsList 'FTFP_BERT' --truthStrategy 'MC15aPlus' --simulator 'FullG4_LongLived' --postInclude 'default:PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py' --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' --DataRunNumber '222525' --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/EVNT.04607198._000001.pool.root.1" --outputHITSFile "test.HITS.pool.root" --maxEvents 10

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
