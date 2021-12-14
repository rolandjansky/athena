#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF, generating events on-the-fly, using 2015 geometry and conditions
# art-include: master/Athena
# art-type: grid
# art-output: *.HITS.pool.root
# art-output: *.TR.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '1500' \
--randomSeed '1234' \
--DataRunNumber '222525' \
--CosmicFilterVolume 'Calo' \
--CosmicFilterVolume2 'NONE' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--CosmicPtSlice 'NONE' \
--outputEVNT_TRFile 'test.CA.TR.pool.root' \
--beamType 'cosmics' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier,CosmicGenerator.CosmicGeneratorConfig.postIncludeCosmicGenerator' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--truthStrategy 'MC12' \
--imf False

rc=$?
mv log.AtlasG4Tf log.AtlasG4Tf_CA
echo  "art-result: $rc simCA"

AtlasG4_tf.py \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '1500' \
--randomSeed '1234' \
--DataRunNumber '222525' \
--CosmicFilterVolume 'Calo' \
--CosmicFilterVolume2 'NONE' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--CosmicPtSlice 'NONE' \
--outputEVNT_TRFile 'test.CA.TR.pool.root' \
--beamType 'cosmics' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--truthStrategy 'MC12' \
--imf False \
--athenaopts '"--config-only=ConfigSimCG.pkl"'

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
--truthStrategy 'MC12' \
--imf False

rc2=$?
mv log.AtlasG4Tf log.AtlasG4Tf_OLD
echo  "art-result: $rc2 simOLD"

rc3=-9999
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.TR.pool.root test.CA.TR.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 TR_OLDvsCA"

rc4=-9999
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root test.CA.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 HITS_OLDvsCA"

rc5=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file=test.HITS.pool.root --file=test.TR.pool.root
    rc5=$?
fi
echo  "art-result: $rc5 regression"
