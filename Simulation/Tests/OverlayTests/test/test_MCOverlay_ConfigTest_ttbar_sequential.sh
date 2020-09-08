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

Overlay_tf.py \
--CA \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup-MT.100events.pool.root \
--outputRDOFile mcOverlayRDO.pool.root \
--maxEvents 10 \
--conditionsTag OFLCOND-MC16-SDR-20 \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--postInclude 'OverlayConfiguration.OverlayTestHelpers.JobOptsDumperCfg' \
--postExec 'with open("ConfigOverlay.pkl", "wb") as f: acc.store(f)' \
--imf False

rc=$?
echo "art-result: $rc overlay"

rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --order-trees
    rc2=$?
fi
echo  "art-result: $rc2 regression"
