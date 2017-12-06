#!/bin/sh

# art-description: OverlayTrf for data16.
# art-type: grid
# art-ci: 21.0, master

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

Reco_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.424000.ParticleGun_single_mu_Pt100.simul.HITS.e3580_s3126/HITS.11330296._000376.pool.root.1 --inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/user.tkharlam.PileupPremixing.r21.0.22.159000.ParticleGenerator_nu_E50.pileup.mc16a_caseA_EXT0/user.tkharlam.11805093.EXT0._000346.RDO.pool.root --outputRDOFile MC_plus_MC.RDO.pool.root --maxEvents 20 --skipEvents 10  --digiSeedOffset1 511 --digiSeedOffset2 727 --conditionsTag OFLCOND-MC16-SDR-14 --geometryVersion ATLAS-R2-2016-01-00-01 --triggerConfig 'OverlayPool=NONE' --postExec 'ServiceMgr.TGCcablingServerSvc.forcedUse=True' --preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' --steering="doOverlay"

JOBSTATUS_1=$?

Reco_tf.py --inputRDOFile MC_plus_MC.RDO.pool.root --outputESDFile MC_plus_MC.ESD.pool.root --outputAODFile MC_plus_MC.AOD.pool.root --maxEvents -1 --skipEvents 0 --autoConfiguration everything --preExec 'rec.doTrigger=False;from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True);' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False'

JOBSTATUS_2=$?

SCRIPT_DIRECTORY=$1 
PACKAGE=$2 
TYPE=$3 
TEST_NAME=$4 
NIGHTLY_RELEASE=$5 
PROJECT=$6 
PLATFORM=$7 
NIGHTLY_TAG=$8 

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME MC_plus_MC.RDO.pool.root MC_plus_MC.ESD.pool.root MC_plus_MC.AOD.pool.root
JOBSTATUS_3=$?

echo "art-result: [${JOBSTATUS_1}, ${JOBSTATUS_2}, ${JOBSTATUS_3}]"