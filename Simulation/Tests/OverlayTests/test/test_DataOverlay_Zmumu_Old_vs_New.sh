#!/bin/sh

# art-description: MC+data Overlay with MT support, sequential running
# art-type: grid
# art-include: master/Athena

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*


Overlay_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
--inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--outputRDOFile MC_plus_data.NEW.RDO.pool.root \
--triggerConfig 'Overlay=NONE' \
--maxEvents 5 \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--samplingFractionDbTag FTFP_BERT_BIRK \
--fSampltag LARElecCalibMCfSampl-G496-19213- \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postInclude 'EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
--imf False

rc=$?
echo "art-result: $rc overlay_tf"

OverlayBS_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
--inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--outputRDOFile MC_plus_data.OLD.RDO.pool.root \
--maxEvents 5 \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--samplingFractionDbTag FTFP_BERT_BIRK \
--fSampltag LARElecCalibMCfSampl-G496-19213- \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postInclude 'EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--postExec 'outStream.ItemList.remove("xAOD::EventInfoContainer#*"); outStream.ItemList.remove("xAOD::EventInfoAuxContainer#*");' \
--imf False

rc2=$?
echo "art-result: $rc2 overlaybs_tf"


rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root MC_plus_data.OLD.RDO.pool.root MC_plus_data.NEW.RDO.pool.root \
        --mode=semi-detailed \
        --ignore-leaves index_ref \
            RecoTimingObj_p1_HITStoRDO_timings.timings \
            RecoTimingObj_p1_EVNTtoHITS_timings.timings \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.subEventIndex \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.subEventTime \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.subEventType \
            xAOD::EventAuxInfo_v2_EventInfoAux.detectorMask0 \
            xAOD::EventAuxInfo_v2_EventInfoAux.detectorMask1 \
            xAOD::EventAuxInfo_v2_EventInfoAux.detectorMask2 \
            xAOD::EventAuxInfo_v2_EventInfoAux.detectorMask3 \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.actualInteractionsPerCrossing \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.averageInteractionsPerCrossing \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.pileUpMixtureIDLowBits \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.pileUpMixtureIDHighBits
    rc3=$?
fi
echo "art-result: $rc3 comparison"
