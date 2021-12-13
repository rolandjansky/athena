#!/bin/sh
#
# art-description: Run simulation outside ISF, reading min bias events, write cavern background track records, using 2015 geometry and conditions
# art-type: grid
# art-include: master/Athena
# art-output: *.EVNT_TR.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic_low-pythia8-7000.evgen.pool.root' \
--outputHITSFile 'discard.HITS.pool.root' \
--outputEVNT_TRFile 'test.CA.EVNT_TR.pool.root' \
--maxEvents '2' \
--skipEvents '0' \
--randomSeed '5678' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'QGSP_BERT_HP' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--truthStrategy 'MC12' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.AtlasG4Tf log.AtlasG4Tf_CA
echo  "art-result: $rc simCA"

AtlasG4_tf.py \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic_low-pythia8-7000.evgen.pool.root' \
    --outputHITSFile 'discard.HITS.pool.root' \
    --outputEVNT_TRFile 'test.CA.EVNT_TR.pool.root' \
    --maxEvents '2' \
    --skipEvents '0' \
    --randomSeed '5678' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --DataRunNumber '222525' \
    --physicsList 'QGSP_BERT_HP' \
    --postInclude 'PyJobTransforms/UseFrontier.py' \
    --truthStrategy 'MC12' \
    --imf False \
    --athenaopts '"--config-only=ConfigSimCG.pkl"'

AtlasG4_tf.py \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic_low-pythia8-7000.evgen.pool.root' \
    --outputHITSFile 'discard.HITS.pool.root' \
    --outputEVNT_TRFile 'test.EVNT_TR.pool.root' \
    --maxEvents '2' \
    --skipEvents '0' \
    --randomSeed '5678' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --DataRunNumber '222525' \
    --physicsList 'QGSP_BERT_HP' \
    --postInclude 'PyJobTransforms/UseFrontier.py' \
    --truthStrategy 'MC12' \
    --imf False

rc2=$?
mv log.AtlasG4Tf log.AtlasG4Tf_OLD
echo  "art-result: $rc2 simOLD"

# remove HITS file
rm discard.HITS.pool.root

rc3=-9999
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.EVNT_TR.pool.root test.CA.EVNT_TR.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 OLDvsCA"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 2 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file=test.EVNT_TR.pool.root
    rc4=$?
fi
echo  "art-result: $rc4 regression"
