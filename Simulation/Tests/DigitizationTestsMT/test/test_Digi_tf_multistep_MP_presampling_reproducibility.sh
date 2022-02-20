#!/bin/bash
#
# art-description: Run multistep pile-up presampling
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena
# art-output: multistep_presampling_SP.RDO.pool.root
# art-output: multistep_presampling_MP_fork_evt0.RDO.pool.root
# art-output: multistep_presampling_MP_fork_evt1.RDO.pool.root

export ATHENA_CORE_NUMBER=8

Events=100
DigiOutFileNameSP="multistep_presampling_SP.RDO.pool.root"
DigiOutFileNameMP0="multistep_presampling_MP_fork_evt0.RDO.pool.root"
DigiOutFileNameMP1="multistep_presampling_MP_fork_evt1.RDO.pool.root"

HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.900149.PG_single_nu_Pt50.simul.HITS.e8307_s3482/HITS.24078104._234467.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/*"


Digi_tf.py \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameSP} \
--numberOfLowPtMinBias 22.4 \
--numberOfHighPtMinBias 0.1 \
--pileupInitialBunch -32 \
--pileupFinalBunch 6 \
--preExec 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.OldBeamSpotZSize.set_Value_and_Lock(0);' \
--preInclude 'HITtoRDO:Campaigns/MC20e.py,RunDependentSimData/configEvtNbr_sequential.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000_splitting.py' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py SP"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 0 \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameMP0} \
--numberOfLowPtMinBias 22.4 \
--numberOfHighPtMinBias 0.1 \
--pileupInitialBunch -32 \
--pileupFinalBunch 6 \
--preExec 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.OldBeamSpotZSize.set_Value_and_Lock(0);' \
--preInclude 'HITtoRDO:Campaigns/MC20e.py,RunDependentSimData/configEvtNbr_sequential.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000_splitting.py' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--skipEvents 0

rc2=$?
if [[ $status -eq 0 ]]; then
  status=$rc2
fi
echo "art-result: $rc2 Digi_tf.py MP fork after 0"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 1 \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameMP1} \
--numberOfLowPtMinBias 22.4 \
--numberOfHighPtMinBias 0.1 \
--pileupInitialBunch -32 \
--pileupFinalBunch 6 \
--preExec 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.OldBeamSpotZSize.set_Value_and_Lock(0);' \
--preInclude 'HITtoRDO:Campaigns/MC20e.py,RunDependentSimData/configEvtNbr_sequential.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000_splitting.py' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--skipEvents 0

rc3=$?
if [[ $status -eq 0 ]]; then
  status=$rc3
fi
echo "art-result: $rc3 Digi_tf.py MP fork after 1"

rc4=-9999
if [[ $status -eq 0 ]] && [[ $rc -eq 0 ]] && [[ $rc2 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameSP} ${DigiOutFileNameMP0} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc4=$?
    status=$rc4
fi
echo "art-result: $rc4 SP vs MP fork after 0"

rc5=-9999
if [[ $status -eq 0 ]] && [[ $rc -eq 0 ]] && [[ $rc3 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameSP} ${DigiOutFileNameMP1} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc5=$?
    status=$rc5
fi
echo "art-result: $rc5 SP vs MP fork after 1"

exit $status
