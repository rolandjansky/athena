#!/bin/sh

# art-description: MC+MC Overlay for MC16, single mu, no trigger
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
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.424000.ParticleGun_single_mu_Pt100.simul.HITS.e3580_s3126/HITS.11330296._000376.pool.root.1 \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/21.0/v1/RDO.merged-pileup.100events.pool.root \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--maxEvents 20 --skipEvents 10  --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-20 \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--triggerConfig 'OverlayPool=NONE' \
--postExec 'ServiceMgr.TGCcablingServerSvc.forcedUse=True' \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
--postInclude 'EventOverlayJobTransforms/blockFoldersMC_OverlayPool.py' \
--imf False
echo "art-result: $? overlaypool"

Reco_tf.py \
--inputRDOFile MC_plus_MC.RDO.pool.root \
--outputESDFile MC_plus_MC.ESD.pool.root \
--outputAODFile MC_plus_MC.AOD.pool.root \
--maxEvents -1 --skipEvents 0 --autoConfiguration everything \
--preExec 'rec.doTrigger=False;from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' \
--postInclude 'r2e:EventOverlayJobTransforms/blockFoldersMC_RAWtoESD.py' 'e2a:EventOverlayJobTransforms/blockFoldersMC_ESDtoAOD.py' \
--imf False
echo "art-result: $? reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? regression"
