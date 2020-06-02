#!/bin/sh
#
# art-description: Test of DigiMReco from HITS to AOD/TAG MC16a r10724
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py --maxEvents=10 --outputRDOFile=mc16a.RDO.pool.root --outputESDFile=mc16a.ESD.pool.root --outputAODFile=mc16a.AOD.pool.root --digiSeedOffset1=1 --digiSeedOffset2=2 --inputHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e5458_s3126/HITS.10730668._039360.pool.root.1 --jobNumber=1 --conditionsTag="default:OFLCOND-MC16-SDR-25" --geometryVersion="default:ATLAS-R2-2016-01-00-01" --skipEvents=0  --pileupFinalBunch=6 --autoConfiguration=everything --digiSteeringConf=StandardSignalOnlyTruth --valid=True --preInclude "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000.py" "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_mc16a.py" --preExec "all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)" "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False); recAlgs.doTrigger.set_Value_and_Lock(False);" "ESDtoAOD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.AODEDMSet.set_Value_and_Lock(\"AODSLIM\");" --postExec "all:CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"]" "ESDtoAOD:fixedAttrib=[s if \"CONTAINER_SPLITLEVEL = \'99\'\" not in s else \"\" for s in svcMgr.AthenaPoolCnvSvc.PoolAttributes];svcMgr.AthenaPoolCnvSvc.PoolAttributes=fixedAttrib" --postInclude "default:PyJobTransforms/UseFrontier.py" --LowPtMinbiasHitsFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000027.pool.root.1 --HighPtMinbiasHitsFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/HITS.10501933._000008.pool.root.1 --numberOfHighPtMinBias=0.2595392 --numberOfLowPtMinBias=99.2404608 --numberOfCavernBkg=0


RES=$?
echo "art-result: $RES Reco"
return $RES

