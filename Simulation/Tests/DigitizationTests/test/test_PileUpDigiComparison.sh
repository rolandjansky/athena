#!/bin/sh
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approch.
# art-type: grid
# art-include: master/Athena
# art-output: mc16a_ttbar.CG.RDO.pool.root
# art-output: mc16a_ttbar.CA.RDO.pool.root
# art-output: log.*

DigitizationPUConfigNew_test.py
rc=$?
echo  "art-result: $rc CAdigi"

DigiOutFileName="mc16d_ttbar.CG.RDO.pool.root"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"

rc2=-9999
if [ $rc -eq 0 ]
then
    # config only
    Digi_tf.py \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
    --conditionsTag default:OFLCOND-MC16-SDR-20 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --jobNumber 1 \
    --maxEvents 4 \
    --numberOfCavernBkg 0 \
    --numberOfHighPtMinBias 0.2099789464 \
    --numberOfLowPtMinBias 80.290021063135 \
    --outputRDOFile ${DigiOutFileName} \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' 'job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --pileupFinalBunch 6 \
    --preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)' \
    --preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run300000_mc16d.py' \
    --skipEvents 0 \
    --athenaopts '"--config-only=DigiPUConfigCG.pkl"'
    # full run
    Digi_tf.py \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
    --conditionsTag default:OFLCOND-MC16-SDR-20 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --jobNumber 1 \
    --maxEvents 4 \
    --numberOfCavernBkg 0 \
    --numberOfHighPtMinBias 0.2099789464 \
    --numberOfLowPtMinBias 80.290021063135 \
    --outputRDOFile ${DigiOutFileName} \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' 'job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --pileupFinalBunch 6 \
    --preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)' \
    --preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run300000_mc16d.py' \
    --skipEvents 0
fi

echo  "art-result: $rc2 STdigi"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root mc16d_ttbar.CG.RDO.pool.root mc16d_ttbar.CA.RDO.pool.root --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref --error-mode resilient
    rc3=$?
fi

echo  "art-result: $rc3 comparison"
