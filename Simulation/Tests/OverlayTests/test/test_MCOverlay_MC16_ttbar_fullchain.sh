#!/bin/sh

# art-description: MC+MC Overlay chain for MC16, ttbar, full reco chain
# art-type: grid

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

Reco_tf.py \
--AMIConfig r11326 \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup.100events.pool.root \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--outputESDFile MC_plus_MC.ESD.pool.root \
--outputAODFile MC_plus_MC.AOD.pool.root \
--maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--imf False

rc=$?
echo "art-result: $rc reco"

rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --file MC_plus_MC.RDO.pool.root --diff-root
    rc2=$?
fi
echo "art-result: $rc2 regression"
