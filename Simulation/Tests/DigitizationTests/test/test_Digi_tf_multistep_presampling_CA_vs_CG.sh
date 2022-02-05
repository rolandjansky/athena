#!/bin/sh
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-include: master/Athena
# art-output: multistep_presampling.CG.RDO.pool.root
# art-output: multistep_presampling.CA.RDO.pool.root
# art-output: log.*
# art-output: legacy.*
# art-output: DigiPUConfig*

Events=100
DigiOutFileNameCG="multistep_presampling.CG.RDO.pool.root"
DigiOutFileNameCA="multistep_presampling.CA.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.900149.PG_single_nu_Pt50.simul.HITS.e8307_s3482/HITS.24078104._234467.pool.root.1"
HighPtMinbiasHitsFiles1="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106512._000149.pool.root.1"
HighPtMinbiasHitsFiles2="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106512._000581.pool.root.1"
HighPtMinbiasHitsFiles3="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106512._000717.pool.root.1"
LowPtMinbiasHitsFiles1="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106626._000068.pool.root.1"
LowPtMinbiasHitsFiles2="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106626._000480.pool.root.1"
LowPtMinbiasHitsFiles3="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106626._000574.pool.root.1"


# config only
Digi_tf.py \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles1} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles2} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles3} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles1} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles2} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles3} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--numberOfLowPtMinBias 22.4 \
--numberOfHighPtMinBias 0.1 \
--pileupInitialBunch -32 \
--pileupFinalBunch 6 \
--preExec 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.OldBeamSpotZSize.set_Value_and_Lock(0);digitizationFlags.HighPtMinBiasInputColOffset=-1;' \
--preInclude 'HITtoRDO:Campaigns/MC20e.py,RunDependentSimData/configEvtNbr_sequential.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000_splitting.py' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--skipEvents 15 \
--athenaopts '"--config-only=DigiPUConfigCG.pkl"'

# full run
Digi_tf.py \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles1} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles2} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles3} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles1} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles2} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles3} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--numberOfLowPtMinBias 22.4 \
--numberOfHighPtMinBias 0.1 \
--pileupInitialBunch -32 \
--pileupFinalBunch 6 \
--preExec 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.OldBeamSpotZSize.set_Value_and_Lock(0);digitizationFlags.HighPtMinBiasInputColOffset=-1;' \
--preInclude 'HITtoRDO:Campaigns/MC20e.py,RunDependentSimData/configEvtNbr_sequential.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000_splitting.py' \
--postExec 'HITtoRDO:job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--skipEvents 15

rc=$?
status=$rc
echo "art-result: $rc CGdigi"
mv runargs.HITtoRDOExecutorStep0.py runargs.legacy.HITtoRDOExecutorStep0.py
mv runargs.HITtoRDOExecutorStep1.py runargs.legacy.HITtoRDOExecutorStep1.py
mv runargs.HITtoRDOExecutorStep2.py runargs.legacy.HITtoRDOExecutorStep2.py
mv runargs.RDOMergeAthenaMP0.py runargs.legacy.RDOMergeAthenaMP0.py
mv log.HITtoRDOExecutorStep0 legacy.HITtoRDOExecutorStep0
mv log.HITtoRDOExecutorStep1 legacy.HITtoRDOExecutorStep1
mv log.HITtoRDOExecutorStep2 legacy.HITtoRDOExecutorStep2
mv log.RDOMergeAthenaMP0 legacy.RDOMergeAthenaMP0

rc2=-9999
if [ $rc -eq 0 ]
then
    Digi_tf.py \
    --CA 'HITtoRDO:True' \
    --splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
    --detectors Truth \
    --PileUpPresampling True \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --digiSteeringConf 'StandardSignalOnlyTruth' \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --inputHITSFile ${HSHitsFile} \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles1} \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles2} \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles3} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles1} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles2} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles3} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --numberOfLowPtMinBias 22.4 \
    --numberOfHighPtMinBias 0.1 \
    --postInclude 'HITtoRDO:PyJobTransforms.UseFrontier' 'HITtoRDO:Digitization.DigitizationSteering.DigitizationTestingPostInclude' \
    --preInclude 'HITtoRDO:Campaigns.MC20e' \
    --preExec 'HITtoRDO:ConfigFlags.Digitization.PU.ProfileConfig="RunDependentSimData.PileUpProfile_run310000_splitting";' \
    --skipEvents 15

    rc2=$?
    status=$rc2
fi
echo  "art-result: $rc2 CAdigi"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root ${DigiOutFileNameCG} ${DigiOutFileNameCA} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo  "art-result: $rc3 comparison"

exit $status
