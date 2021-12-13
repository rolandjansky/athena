#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF, using TrackRecords as input, using 2015 geometry and conditions
# art-type: grid
# art-include: master/Athena
# art-output: *.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/Cosmics.TR.pool.root' \
--outputHITSFile 'test.CA.HITS.pool.root' \
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
mv log.AtlasG4TfTRIn log.AtlasG4TfTRIn_CA
echo  "art-result: $rc simCA"

AtlasG4_tf.py \
--inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/Cosmics.TR.pool.root' \
--outputHITSFile 'test.CA.HITS.pool.root' \
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
-athenaopts '"--config-only=ConfigSimCG.pkl"'

AtlasG4_tf.py \
--inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/Cosmics.TR.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
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

rc2=$?
mv log.AtlasG4TfTRIn log.AtlasG4TfTRIn_OLD
echo  "art-result: $rc2 simOLD"

rc3=-9999
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root test.CA.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 OLDvsCA"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file=test.HITS.pool.root
    rc4=$?
fi
echo  "art-result: $rc4 regression"
