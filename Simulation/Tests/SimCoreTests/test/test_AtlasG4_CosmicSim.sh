#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF, generating events on-the-fly, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: test.TR.pool.root

AtlasG4_tf.py \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '1500' \
--randomSeed '1234' \
--DataRunNumber '222525' \
--CosmicFilterVolume 'Calo' \
--CosmicFilterVolume2 'NONE' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--CosmicPtSlice 'NONE' \
--outputEVNT_TRFile 'test.TR.pool.root' \
--beamType 'cosmics' \
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
