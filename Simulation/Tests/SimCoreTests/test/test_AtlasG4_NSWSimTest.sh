#!/bin/sh
#
# art-description: Tests NSW detector functionality using di-muon events, generating events on-the-fly
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

AtlasG4_tf.py \
--preInclude 'G4AtlasTests/ParticleGun_flatpt_2particle.py' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '200' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-25' \
--physicsList 'FTFP_BERT' \
--DBRelease 'current' \
--postInclude 'G4AtlasTests/postInclude.DCubeTest.py,G4AtlasTests/postInclude.NSW.config.simu.py' \
--preExec 'simFlags.ReleaseGeoModel=False;simFlags.SimulateNewSmallWheel=True;' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed

echo  "art-result: $? regression"
