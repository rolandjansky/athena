#!/bin/sh
#
# art-description: Run simulation outside ISF, reading di-jet events, using FTFP_BERT_ATL physics list, writing HITS, using 2015 geometry and conditions
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: *.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT_ATL' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/J2_jetjet-pythia6-7000.evgen.pool.root' \
--outputHITSFile "test.CA.HITS.pool.root" \
--maxEvents '10' \
--skipEvents '0' \
--randomSeed '10' \
--truthStrategy 'MC15aPlus' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.AtlasG4Tf log.AtlasG4Tf_CA
echo  "art-result: $rc simCA"
rc2=-9999
if [ $rc -eq 0 ]
then
    AtlasG4_tf.py \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT_ATL' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --DataRunNumber '222525' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/J2_jetjet-pythia6-7000.evgen.pool.root' \
    --outputHITSFile "test.HITS.pool.root" \
    --maxEvents '10' \
    --skipEvents '0' \
    --randomSeed '10' \
    --truthStrategy 'MC15aPlus' \
    --imf False \
    --athenaopts '"--config-only=ConfigSimCG.pkl"'

    AtlasG4_tf.py \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT_ATL' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --DataRunNumber '222525' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/J2_jetjet-pythia6-7000.evgen.pool.root' \
    --outputHITSFile "test.HITS.pool.root" \
    --maxEvents '10' \
    --skipEvents '0' \
    --randomSeed '10' \
    --truthStrategy 'MC15aPlus' \
    --imf False
    rc2=$?
    mv log.AtlasG4Tf log.AtlasG4Tf_OLD
fi
echo  "art-result: $rc2 simOLD"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root test.CA.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 FullG4MT_OLDvsCA"
