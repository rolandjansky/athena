#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --digiSteeringConf 'StandardSignalOnlyTruth' --conditionsTag 'default:OFLCOND-MC16-SDR-17' --valid 'True' --pileupFinalBunch '6' --numberOfHighPtMinBias '0.152593389' --autoConfiguration 'everything' --numberOfLowPtMinBias '58.3474065525' --steering 'doRDO_TRIG' --preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_mc16a.py' --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'ESDtoAOD:fixedAttrib=[s if "CONTAINER_SPLITLEVEL = \"99\"" not in s else "" for s in svcMgr.AthenaPoolCnvSvc.PoolAttributes];svcMgr.AthenaPoolCnvSvc.PoolAttributes=fixedAttrib' 'RDOtoRDOTrigger:conddb.addOverride("/CALO/Ofl/Noise/PileUpNoiseLumi","CALOOflNoisePileUpNoiseLumi-mc15-mu30-dt25ns")' --postInclude 'default:PyJobTransforms/UseFrontier.py' --preExec 'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' 'all:from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doWriteTracksToESD.set_Value_and_Lock(True)' 'ESDtoAOD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.AODEDMSet.set_Value_and_Lock("AODFULL");' 'RDOtoRDOTrigger:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup.set_Value_and_Lock("MC_pp_v6_tight_mc_prescale");' 'RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup.set_Value_and_Lock("MC_pp_v6_tight_mc_prescale");' 'all:from TriggerJobOpts.TriggerFlags import TriggerFlags as TF;TF.run2Config="2016"' --geometryVersion 'default:ATLAS-R2-2016-01-00-01' --numberOfCavernBkg '0' --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/* --maxEvents=100 --outputAODFile=myAOD.pool.root --outputRDOFile="myRDO.pool.root" --outputESDFile="myESD.pool.root" --outputTAGFile="myTAG.root" --runNumber=410000 --jobNumber=1 --inputLowPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/* --inputHighPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/* --imf False
echo "art-result: $?"

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD.pool.root myESD.pool.root
echo "art-result: $?"

art.py compare grid --days=3 $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD.pool.root myESD.pool.root
echo "art-result: $?"