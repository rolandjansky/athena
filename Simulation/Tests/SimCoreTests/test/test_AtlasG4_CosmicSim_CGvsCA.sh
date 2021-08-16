#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF, generating events on-the-fly, using 2015 geometry and conditions
# art-type: grid
# art-output: test.*.HITS.pool.root
# art-output: test.*.TR.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--outputHITSFile 'test.NEW.HITS.pool.root' \
--maxEvents '1500' \
--randomSeed '1234' \
--DataRunNumber '222525' \
--CosmicFilterVolume 'Calo' \
--CosmicFilterVolume2 'NONE' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--CosmicPtSlice 'NONE' \
--outputEVNT_TRFile 'test.NEW.TR.pool.root' \
--beamType 'cosmics' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier,CosmicGenerator.CosmicGeneratorConfig.postIncludeCosmicGenerator' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--truthStrategy 'MC12' \
--imf False

# Currently generates errors in ID DetectorManger initialisations. Unclear why.

rc=$?
mv log.AtlasG4Tf log.G4AtlasAlg_AthenaCA
echo  "art-result: $rc G4AtlasAlg_AthenaCA"
rc2=-9999
if [ $rc -eq 68 ]
then
    AtlasG4_tf.py \
    --outputHITSFile 'test.NEW.HITS.pool.root' \
    --maxEvents '1500' \
   --randomSeed '1234' \
   --DataRunNumber '222525' \
   --CosmicFilterVolume 'Calo' \
   --CosmicFilterVolume2 'NONE' \
   --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
   --conditionsTag 'OFLCOND-RUN12-SDR-19' \
   --physicsList 'FTFP_BERT' \
   --CosmicPtSlice 'NONE' \
   --outputEVNT_TRFile 'test.NEW.TR.pool.root' \
   --beamType 'cosmics' \
   --postInclude 'PyJobTransforms/UseFrontier.py' \
   --truthStrategy 'MC12' \
   --imf False \
   --athenaopts '"--config-only=ConfigSimCG.pkl"'

    AtlasG4_tf.py \
    --outputHITSFile 'test.OLD.HITS.pool.root' \
    --maxEvents '1500' \
   --randomSeed '1234' \
   --DataRunNumber '222525' \
   --CosmicFilterVolume 'Calo' \
   --CosmicFilterVolume2 'NONE' \
   --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
   --conditionsTag 'OFLCOND-RUN12-SDR-19' \
   --physicsList 'FTFP_BERT' \
   --CosmicPtSlice 'NONE' \
   --outputEVNT_TRFile 'test.OLD.TR.pool.root' \
   --beamType 'cosmics' \
   --postInclude 'PyJobTransforms/UseFrontier.py' \
   --truthStrategy 'MC12' \
   --imf False

    rc2=$?
fi

echo  "art-result: $rc2 G4AtlasAlg_AthenaCA_OLD"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.OLD.TR.pool.root test.NEW.TR.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_AtlasG4Tf_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 CompareTR_CGvsCA"
rc5=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.OLD.HITS.pool.root test.NEW.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_AtlasG4Tf_timings index_ref
    rc5=$?
fi
echo  "art-result: $rc5 CompareHITS_CGvsCA"
