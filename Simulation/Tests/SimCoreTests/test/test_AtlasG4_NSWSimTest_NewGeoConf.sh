#!/bin/sh
#
# art-description: Tests NSW detector functionality using di-muon events, generating events on-the-fly
# art-type: grid

AtlasG4_tf.py --preInclude 'G4AtlasTests/ParticleGun_flatpt_2particle.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents '200' --randomSeed '10' --geometryVersion 'ATLAS-R3-2021-00-00-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-25' --physicsList 'FTFP_BERT' --postInclude 'G4AtlasTests/postInclude.DCubeTest.py,PyJobTransforms/UseFrontier.py' --preExec 'simFlags.ReleaseGeoModel=False;'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
