#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/Athena
# art-description: Pythia event generation -- Zprime to tautau
# art-architecture: '#x86_64-intel'
# art-type: grid
# art-output: *.root

Sim_tf.py --AMIConfig s3822 --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/EVGEN_21p6/mgpy8eg_ttbar_13p6.EVGEN.root --maxEvents 10 --runNumber 421452 --firstEvent 759001 --randomSeed 760 --outputHITSFile HITS.28723600._001356.pool.root.1 --physicsList FTFP_BERT_ATL_VALIDATION --truthStrategy MC15aPlus --conditionsTag 'OFLCOND-MC21-SDR-RUN3-05' --geometryVersion 'ATLAS-R3S-2021-02-00-00_VALIDATION' --preExec 'simFlags.CalibrationRun.set_Off();' --postExec 'conddb.addOverride("/Indet/Beampos","IndetBeampos-RunDep-MC21-BestKnowledge-002");' --preInclude 'SimulationJobOptions/preInclude.BeamPipeKill.py' 'SimulationJobOptions/preInclude.G4Optimizations.py' 'SimulationJobOptions/preInclude.ExtraParticles.py' 'SimulationJobOptions/preInclude.G4ExtraProcesses.py' --postInclude 'RecJobTransforms/UseFrontier.py' 'Campaigns/postInclude.MC21BirksConstantTune.py'

echo "art-result: $? Sim_tf"

export ATHENA_CORE_NUMBER=8
Reco_tf.py --inputHITSFile=HITS.28723600._001356.pool.root.1 --athenaMPEventsBeforeFork=1 --athenaopts "HITtoRDO:--nprocs=$ATHENA_CORE_NUMBER" "HITtoRDO:--threads=0" --deleteIntermediateOutputfiles=True --maxEvents=10 --multithreaded=True --postExec "all:conddb.addOverride(\"/Indet/Beampos\",\"IndetBeampos-RunDep-MC21-BestKnowledge-002\")" "all:from IOVDbSvc.CondDB import conddb; conddb.addOverride(\"/TRT/Calib/PID_NN\", \"TRTCalibPID_NN_v2\")" "all:conddb.addOverride(\"/PIXEL/ChargeCalibration\",\"PixelChargeCalibration-SIM-MC16-000-12\")" --postInclude "default:PyJobTransforms/UseFrontier.py" --preExec "HITtoRDO:userRunLumiOverride={\"run\":330000, \"startmu\":25.0,\"endmu\":52.0,\"stepmu\":1,\"startlb\":1,\"timestamp\":1560000000};from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.doPixelPlanarRadiationDamage=True" "all:from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.AODEDMSet='AODFULL'" --preInclude "all:Campaigns/MC21a.py" "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_muRange.py" --skipEvents=0 --autoConfiguration=everything --valid=True --conditionsTag "default:OFLCOND-MC21-SDR-RUN3-04" --geometryVersion="default:ATLAS-R3S-2021-02-00-00" --runNumber=421452 --digiSeedOffset1=10 --digiSeedOffset2=10 --digiSteeringConf='StandardSignalOnlyTruth'  --steering "doRDO_TRIG" "doTRIGtoALL" --inputHighPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Overlay/HITS.Py8EG_minbias_highjet_photon_lepton_28688129._000001.pool.root.1 --inputLowPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Overlay/HITS.Epos_minbias_inelastic_lowjetphoton_28600674._000001.pool.root.1 --numberOfHighPtMinBias=0.103 --numberOfLowPtMinBias=52.397 --pileupFinalBunch=6 --pileupInitialBunch=-32 --outputAODFile=AOD.28723600._001356.pool.root.1 --jobNumber=10 

echo "art-result: $? Reco_tf"
