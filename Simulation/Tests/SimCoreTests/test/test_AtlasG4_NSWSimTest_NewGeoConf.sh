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
--geometryVersion 'ATLAS-R3-2021-01-00-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-25' \
--physicsList 'FTFP_BERT' \
--postInclude 'G4AtlasTests/postInclude.DCubeTest.py,PyJobTransforms/UseFrontier.py' \
--preExec 'simFlags.ReleaseGeoModel=False;' \
--imf False

rc=$?
rc2=-9999
echo  "art-result: $rc simulation"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc2=$?
fi

echo  "art-result: $rc2 regression"
