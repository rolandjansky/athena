#!/bin/sh
#
# art-description: Tests detector response to single pions, generated on-the-fly, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --preInclude 'SimulationJobOptions/preInclude.SinglePionGenerator.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents '150' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest.py' --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;'


ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and run DCube
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
