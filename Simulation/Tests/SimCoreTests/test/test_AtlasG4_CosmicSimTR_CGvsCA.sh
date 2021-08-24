#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF, using TrackRecords as input, using 2015 geometry and conditions
# art-type: grid
# art-output: test.*.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/Cosmics.TR.pool.root' \
--outputHITSFile 'test.NEW.HITS.pool.root' \
--maxEvents '-1' \
--randomSeed '1234' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--DataRunNumber '222525' \
--firstEvent '0' \
--beamType 'cosmics' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--truthStrategy 'MC15aPlus' \
--imf False

rc=$?
mv log.AtlasG4Tf log.G4AtlasAlg_AthenaCA
echo  "art-result: $rc G4AtlasAlg_AthenaCA"
rc2=-9999
if [ $rc -eq 0 ]
then
    AtlasG4_tf.py \
    --inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/Cosmics.TR.pool.root' \
    --outputHITSFile 'test.OLD.HITS.pool.root' \
    --maxEvents '-1' \
    --randomSeed '1234' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT' \
    --DataRunNumber '222525' \
    --firstEvent '0' \
    --beamType 'cosmics' \
    --postInclude 'PyJobTransforms/UseFrontier.py' \
    --truthStrategy 'MC15aPlus' \
    --imf False \
   --athenaopts '"--config-only=ConfigSimCG.pkl"'

    AtlasG4_tf.py \
    --inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/Cosmics.TR.pool.root' \
    --outputHITSFile 'test.OLD.HITS.pool.root' \
    --maxEvents '-1' \
    --randomSeed '1234' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT' \
    --DataRunNumber '222525' \
    --firstEvent '0' \
    --beamType 'cosmics' \
    --postInclude 'PyJobTransforms/UseFrontier.py' \
    --truthStrategy 'MC15aPlus' \
    --imf False
    mv log.AtlasG4TfTRIn log.G4AtlasAlg_AthenaCA_OLD
    rc2=$?
fi

echo  "art-result: $rc2 G4AtlasAlg_AthenaCA_OLD"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.OLD.HITS.pool.root test.NEW.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_AtlasG4Tf_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 CompareHITS_CGvsCA"
