#!/bin/sh
#
# art-description: Run test-beam simulation outside ISF, generating events on-the-fly, writing HITS
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root

TestBeam_tf.py \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '10' \
--Eta '0.35' \
--testBeamConfig 'tbtile' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
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
