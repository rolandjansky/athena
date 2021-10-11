#!/bin/sh
#
# art-description: Run simulation outside ISF, reading cavern background track records, writing cavern background HITS, using 2015 geometry and conditions
# art-type: grid
# art-output: test.*.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/cavernbg-pythia8-7000.evgen.pool.root' \
--outputHITSFile 'test.NEW.HITS.pool.root' \
--maxEvents '5' \
--skipEvents '0' \
--randomSeed '8765' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'QGSP_BERT_HP' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--truthStrategy 'MC12' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.AtlasG4Tf log.G4AtlasAlg_AthenaCA
echo  "art-result: $rc G4AtlasAlg_AthenaCA"
rc2=-9999
if [ $rc -eq 68 ]
then
    AtlasG4_tf.py \
        --inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/cavernbg-pythia8-7000.evgen.pool.root' \
        --outputHITSFile 'test.NEW.HITS.pool.root' \
        --maxEvents '5' \
        --skipEvents '0' \
        --randomSeed '8765' \
        --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
        --conditionsTag 'OFLCOND-RUN12-SDR-19' \
        --DataRunNumber '222525' \
        --physicsList 'QGSP_BERT_HP' \
        --postInclude 'PyJobTransforms/UseFrontier.py' \
        --truthStrategy 'MC12' \
        --imf False \
        --athenaopts '"--config-only=ConfigSimCG.pkl"'

    AtlasG4_tf.py \
        --inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/cavernbg-pythia8-7000.evgen.pool.root' \
        --outputHITSFile 'test.OLD.HITS.pool.root' \
        --maxEvents '5' \
        --skipEvents '0' \
        --randomSeed '8765' \
        --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
        --conditionsTag 'OFLCOND-RUN12-SDR-19' \
        --DataRunNumber '222525' \
        --physicsList 'QGSP_BERT_HP' \
        --postInclude 'PyJobTransforms/UseFrontier.py' \
        --truthStrategy 'MC12' \
        --imf False

    rc2=$?
fi

echo  "art-result: $rc2 G4AtlasAlg_AthenaCA_OLD"

rc5=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.OLD.HITS.pool.root test.NEW.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_AtlasG4Tf_timings index_ref
    rc5=$?
fi
echo  "art-result: $rc5 CompareHITS_CGvsCA"
