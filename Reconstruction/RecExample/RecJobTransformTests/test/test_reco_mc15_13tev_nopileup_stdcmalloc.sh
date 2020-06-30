#!/bin/sh
#
# art-description: Reco_tf for PhysVal, updated with q221 (2017-11-13), no pileup, with stdcmalloc
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --conditionsTag all:OFLCOND-RUN12-SDR-19 --ignoreErrors 'False' --autoConfiguration='everything' --digiSeedOffset2 '1'  --preExec  'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.);from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.AODEDMSet="AODFULL"'  'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["PhysicsList"];'  'RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v7";from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False;' --digiSeedOffset1 '1' --steering 'doRDO_TRIG' --DataRunNumber '222525' --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.410500.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.simul.HITS.e4797_s2726.50events.pool.root --imf False --maxEvents '500' --athenaopts="--stdcmalloc"

RES=$?
echo "art-result: $RES Reco"
return $RES

