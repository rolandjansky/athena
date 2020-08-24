#!/bin/sh

# art-description: MC+data Overlay with MT support, config test
# art-type: grid
# art-include: master/Athena

# art-output: legacyDataOverlayRDO.pool.root
# art-output: dataOverlayRDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*
# art-output: *.pkl
# art-output: *Config.txt

events=2

Overlay_tf.py \
--detectors BCM \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
--inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--outputRDOFile legacyDataOverlayRDO.pool.root \
--maxEvents $events \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--fSampltag LARElecCalibMCfSampl-G496-19213- \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postInclude 'EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--ignorePatterns "L1TopoMenuLoader.+ERROR." \
--imf False \
--athenaopts '"--config-only=ConfigLegacy.pkl"'

Overlay_tf.py \
--detectors BCM \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
--inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--outputRDOFile legacyDataOverlayRDO.pool.root \
--maxEvents $events \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--fSampltag LARElecCalibMCfSampl-G496-19213- \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postExec 'job+=CfgMgr.JobOptsDumperAlg(FileName="OverlayLegacyConfig.txt");' \
--postInclude 'EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--ignorePatterns "L1TopoMenuLoader.+ERROR." \
--imf False

rc=$?
mv log.Overlay log.OverlayLegacy
echo "art-result: $rc configLegacy"

rc2=-9999
if [ $rc -eq 0 ]
then
    Overlay_tf.py \
    --CA \
    --detectors BCM \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
    --inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
    --outputRDOFile dataOverlayRDO.pool.root \
    --maxEvents $events \
    --conditionsTag CONDBR2-BLKPA-2016-12 \
    --postInclude 'OverlayConfiguration.OverlayTestHelpers.JobOptsDumperCfg' \
    --postExec 'with open("ConfigOverlay.pkl", "wb") as f: acc.store(f)' \
    --imf False \
    --athenaopts="--threads=1"
    rc2=$?
    mv log.Overlay log.OverlayTest
fi
echo  "art-result: $rc2 configNew"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root legacyDataOverlayRDO.pool.root dataOverlayRDO.pool.root \
        --error-mode resilient --mode=semi-detailed \
        --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings RecoTimingObj_p1_HITStoRDO_timings index_ref \
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
echo  "art-result: $rc3 comparison"
