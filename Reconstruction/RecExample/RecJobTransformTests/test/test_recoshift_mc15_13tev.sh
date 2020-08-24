#!/bin/sh
#
# art-description: Xiaohu Sun, Carl Gwilliam and Marie-Helene Genest
# art-athena-mt: 4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py  '--inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.410500.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.simul.HITS.e4797_s2726.50events.pool.root' '--jobNumber=64' '--maxEvents=10' '--postInclude=RecJobTransforms/UseFrontier.py' '--preExec' 'rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' 'ESDtoAOD:TriggerFlags.AODEDMSet="AODFULL"' 'RAWtoESD:from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doSlimming.set_Value_and_Lock(False)' 'ESDtoAOD:from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doSlimming.set_Value_and_Lock(False)' 'RDOtoRDOTrigger:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v6_tight_mc_prescale"' '--skipEvents=0' '--autoConfiguration=everything' '--conditionsTag=OFLCOND-MC15c-SDR-09' '--geometryVersion=ATLAS-R2-2015-03-01-00' '--digiSeedOffset1=1' '--digiSeedOffset2=1'  '--steering=doRDO_TRIG'  '--outputAODFile=AOD.pool.root' '--outputRDOFile=RDO.pool.root' '--outputESDFile=ESD.pool.root'  --runNumber="110401" --pileupFinalBunch 6 --inputHighPtMinbiasHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2195/HITS.05608152._002668.pool.root.1" --inputLowPtMinbiasHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_13TeV.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2195/HITS.05608147._000503.pool.root.1"

RES=$?
echo "art-result: $RES Reco"


