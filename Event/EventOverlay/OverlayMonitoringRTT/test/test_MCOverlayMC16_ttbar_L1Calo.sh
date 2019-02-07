#!/bin/sh

# art-description: MC+MC Overlay for MC16, ttbar, with L1Calo, no reco
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

OverlayPool_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/RDO.merged-pileup.100events.pool.root \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--maxEvents 20 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-20 \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--postExec 'ServiceMgr.TGCcablingServerSvc.forcedUse=True' \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
--postInclude 'EventOverlayJobTransforms/blockFoldersMC_OverlayPool.py' \
--imf False
echo "art-result: $? overlaypool"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? regression"
