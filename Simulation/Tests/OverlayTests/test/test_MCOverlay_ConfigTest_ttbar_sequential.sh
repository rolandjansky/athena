#!/bin/sh

# art-description: MC+MC Overlay with MT support, running sequentially, new config
# art-type: grid
# art-include: master/Athena

# art-output: mcOverlayRDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*
# art-output: *.pkl
# art-output: *Config.txt

events=10
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/v1/mc20e_presampling.RDO.pool.root"


Overlay_tf.py \
--CA \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile mcOverlayRDO.pool.root \
--maxEvents ${events} \
--conditionsTag OFLCOND-MC16-SDR-RUN2-09  \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--preInclude 'all:Campaigns.MC20e' \
--postInclude 'OverlayConfiguration.OverlayTestHelpers.OverlayJobOptsDumperCfg' \
--postExec 'with open("ConfigOverlay.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
status=$rc
echo "art-result: $rc overlay"

rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --order-trees
    rc2=$?
    status=$rc2
fi
echo  "art-result: $rc2 regression"

exit $status
