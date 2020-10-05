#!/bin/sh

# art-description: MC+MC Overlay with MT support, running with 8 threads
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

# art-output: MC_plus_MC.MT.RDO.pool.root
# art-output: MC_plus_MC.ST.RDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

export ATHENA_CORE_NUMBER=8

Overlay_tf.py \
--detector Muon \
--multithreaded \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup-MT.100events.pool.root \
--outputRDOFile MC_plus_MC.MT.RDO.pool.root \
--maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-20 \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
--imf False

rc=$?
echo "art-result: $rc overlayMT"
mv log.Overlay log.OverlayMT

rc2=-9999
if [ $rc -eq 0 ]
then
    Overlay_tf.py \
    --detector Muon \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
    --inputRDO_BKGFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup-MT.100events.pool.root \
    --outputRDOFile MC_plus_MC.ST.RDO.pool.root \
    --maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
    --conditionsTag OFLCOND-MC16-SDR-20 \
    --geometryVersion ATLAS-R2-2016-01-00-01 \
    --preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
    --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
    --imf False
    rc2=$?
fi
echo  "art-result: $rc2 overlayST"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root MC_plus_MC.ST.RDO.pool.root MC_plus_MC.MT.RDO.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings RecoTimingObj_p1_HITStoRDO_timings index_ref xAOD::JetAuxContainer_v1_InTimeAntiKt4TruthJetsAux xAOD::JetAuxContainer_v1_InTimeAntiKt4TruthJetsAuxDyn xAOD::JetAuxContainer_v1_OutOfTimeAntiKt4TruthJetsAux xAOD::JetAuxContainer_v1_OutOfTimeAntiKt4TruthJetsAuxDyn
    rc3=$?
fi
echo  "art-result: $rc3 comparison"
