#!/bin/bash

# art-description: MC+MC Overlay with MT support, config test
# art-type: grid
# art-include: master/Athena

# art-output: legacyMcOverlayRDO.pool.root
# art-output: mcOverlayRDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*
# art-output: *.pkl
# art-output: *Config.txt

set -o pipefail

events=2
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/v1/mc20e_presampling.RDO.pool.root"


Overlay_tf.py \
--detectors LAr \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile legacyMcOverlayRDO.pool.root \
--maxEvents $events \
--conditionsTag OFLCOND-MC16-SDR-RUN2-09  \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--preInclude 'all:Campaigns/MC20e.py' \
--imf False \
--athenaopts '"--config-only=ConfigLegacy.pkl"'

Overlay_tf.py \
--detectors LAr \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile legacyMcOverlayRDO.pool.root \
--maxEvents $events \
--conditionsTag OFLCOND-MC16-SDR-RUN2-09  \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--preInclude 'all:Campaigns/MC20e.py' \
--postExec 'job+=CfgMgr.JobOptsDumperAlg(FileName="OverlayLegacyConfig.txt");' \
--imf False

rc=$?
status=$rc
mv log.Overlay log.OverlayLegacy
echo "art-result: $rc configLegacy"

rc2=-9999
if [ $rc -eq 0 ]
then
    Overlay_tf.py \
    --CA \
    --detectors LAr \
    --inputHITSFile ${HITS_File} \
    --inputRDO_BKGFile ${RDO_BKG_File} \
    --outputRDOFile mcOverlayRDO.pool.root \
    --maxEvents $events \
    --conditionsTag OFLCOND-MC16-SDR-RUN2-09  \
    --geometryVersion ATLAS-R2-2016-01-00-01 \
    --preInclude 'all:Campaigns.MC20e' \
    --postInclude 'OverlayConfiguration.OverlayTestHelpers.OverlayJobOptsDumperCfg' \
    --postExec 'with open("ConfigOverlay.pkl", "wb") as f: cfg.store(f)' \
    --imf False \
    --athenaopts="--threads=1"
    rc2=$?
    status=$rc2
    mv log.Overlay log.OverlayTest
fi
echo  "art-result: $rc2 configNew"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root legacyMcOverlayRDO.pool.root mcOverlayRDO.pool.root \
        --error-mode resilient --mode=semi-detailed \
        --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo  "art-result: $rc3 comparison"

exit $status
