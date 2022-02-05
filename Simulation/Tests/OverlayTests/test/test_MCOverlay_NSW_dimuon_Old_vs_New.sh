#!/bin/sh

# art-description: MC+MC Overlay with NSW support, comparison between old and new transforms
# art-type: grid


# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

Overlay_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.DiMuon10_100GeV.HITS.rel_master_2020_12_01_R3S_v01_EXT1/group.det-muon.23437494.EXT1._000011.HITS.pool.root \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/NSW/v1/NSW_premixing_MT.RDO.pool.root \
--outputRDOFile MC_plus_MC.NEW.RDO.pool.root \
--maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-RUN3-02 \
--geometryVersion ATLAS-R3S-2021-01-00-01 \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
  'conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3ASYM-01"); conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3ASYM-01")' \
--imf False

rc=$?
echo "art-result: $rc overlay_tf"

OverlayPool_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.DiMuon10_100GeV.HITS.rel_master_2020_12_01_R3S_v01_EXT1/group.det-muon.23437494.EXT1._000011.HITS.pool.root \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/NSW/v1/NSW_premixing.RDO.pool.root \
--outputRDOFile MC_plus_MC.OLD.RDO.pool.root \
--maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-14 \
--geometryVersion ATLAS-R3S-2021-01-00-01 \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
  'conddb.addOverride("/MDT/RTBLOB","MDTRT_Sim-R3ASYM-01"); conddb.addOverride("/MDT/T0BLOB","MDTT0_Sim-R3ASYM-01")' \
  'outStream.ItemList.remove("xAOD::EventInfoContainer#*"); outStream.ItemList.remove("xAOD::EventInfoAuxContainer#*");' \
--imf False

rc2=$?
status=$rc2
echo "art-result: $rc2 overlaypool_tf"


rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root MC_plus_MC.OLD.RDO.pool.root MC_plus_MC.NEW.RDO.pool.root \
        --mode=semi-detailed \
        --ignore-leaves index_ref \
            RecoTimingObj_p1_HITStoRDO_timings.timings \
            RecoTimingObj_p1_EVNTtoHITS_timings.timings \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.subEventIndex \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.subEventTime \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.subEventType \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.mcChannelNumber \
            xAOD::EventAuxInfo_v2_EventInfoAuxDyn.mcEventNumber
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 comparison"

exit $status
