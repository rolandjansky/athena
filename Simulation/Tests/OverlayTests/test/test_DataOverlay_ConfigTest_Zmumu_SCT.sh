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

set -o pipefail

events=2

OverlayBS_tf.py \
--inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
--outputRDOFile legacyDataOverlayRDO.pool.root \
--maxEvents $events \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--fSampltag LARElecCalibMCfSampl-G496-19213- \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postExec 'outStream.ItemList.remove("xAOD::EventInfoContainer#*"); outStream.ItemList.remove("xAOD::EventInfoAuxContainer#*");' \
--preInclude 'SimulationJobOptions/preInclude.SCTOnlyConfig.py,SimulationJobOptions/preInclude.TruthOnlyConfig.py' \
--postInclude 'EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--ignorePatterns "L1TopoMenuLoader.+ERROR." \
--imf False \
--athenaopts '"--config-only=ConfigLegacy.pkl"'

OverlayBS_tf.py \
--inputBS_SKIMFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root \
--outputRDOFile legacyDataOverlayRDO.pool.root \
--maxEvents $events \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--fSampltag LARElecCalibMCfSampl-G496-19213- \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postExec 'job+=CfgMgr.JobOptsDumperAlg(FileName="OverlayLegacyConfig.txt"); outStream.ItemList.remove("xAOD::EventInfoContainer#*"); outStream.ItemList.remove("xAOD::EventInfoAuxContainer#*");' \
--preInclude 'SimulationJobOptions/preInclude.SCTOnlyConfig.py,SimulationJobOptions/preInclude.TruthOnlyConfig.py' \
--postInclude 'EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--ignorePatterns "L1TopoMenuLoader.+ERROR." \
--imf False

rc=$?
echo "art-result: $rc configLegacy"
mv log.OverlayBS log.OverlayLegacy

rc2=-9999
if [ $rc -eq 0 ]
then
    OverlayTest.py SCT -d -t 1 -n $events 2>&1 | tee log.OverlayTest
    rc2=$?
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
            xAOD::EventAuxInfo_v2_EventInfoAux.actualInteractionsPerCrossing \
            xAOD::EventAuxInfo_v2_EventInfoAux.averageInteractionsPerCrossing
    rc3=$?
fi
echo  "art-result: $rc3 comparison"
