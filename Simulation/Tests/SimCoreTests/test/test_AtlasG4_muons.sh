#!/bin/sh
#
# art-description: Tests detector response to single muons, generated on-the-fly, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --preInclude 'SimulationJobOptions/preInclude.SingleMuonGenerator.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents '2000' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest.py' --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and run DCube
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}

# TODO Remake plots by reading in the HITS file produced by the previous job (Tests TP conversion) - need to run DCube on output
# athena G4AtlasTests/test_AtlasG4_muons_noevgen.py


