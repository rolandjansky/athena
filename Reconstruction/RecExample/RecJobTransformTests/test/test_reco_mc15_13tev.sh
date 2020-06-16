#!/bin/sh
#
# art-description: Reco_tf for PhysVal, updated with q221 (2017-11-13) + pileup
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --conditionsTag all:OFLCOND-RUN12-SDR-19 --ignoreErrors 'False' --autoConfiguration='everything' --digiSeedOffset2 '1'  --preExec  'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.);from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.AODEDMSet="AODFULL"'  'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["PhysicsList"];'  'RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v7";from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False;' --digiSeedOffset1 '1' --steering 'doRDO_TRIG' --DataRunNumber '222525' --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.410500.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.simul.HITS.e4797_s2726.50events.pool.root --inputHighPtMinbiasHitsFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2195/*' --inputLowPtMinbiasHitsFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2195/*' --numberOfCavernBkg="0" --numberOfHighPtMinBias="0.12268057" --numberOfLowPtMinBias="39.8773194" --pileupFinalBunch 6 --imf False --maxEvents '500'
RES=$?
echo "art-result: $RES Reco"
if [ $RES -ne 0 ];then
return $RES
fi

Reco_tf.py --inputAODFile myAOD.pool.root --outputNTUP_PHYSVALFile PhysVal_13TeV_Example.pool.root --validationFlags doExample
RES=$?
echo "art-result: $RES PhysVal"
if [ $RES -ne 0 ];then
return $RES
fi

Reco_tf.py --validationFlags 'doExample,doMET,doPFlow,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
RES=$?
echo "art-result: $RES Ntup"
return $RES
