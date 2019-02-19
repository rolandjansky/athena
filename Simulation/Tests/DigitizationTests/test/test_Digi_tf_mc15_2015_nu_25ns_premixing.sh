#!/bin/sh
#
# art-description: Run pile-up premixing with a MC15 nu sample with 2015 geometry and conditions, 25ns pile-up
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: mc15_2015_nu_25ns_premixing.RDO.pool.root


DigiOutFileName="mc15_2015_nu_25ns_premixing.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc15a/mc15_valid.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2169_tid05098387_00/HITS.05098387.*00067*.pool.root.?"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc15a/mc15_valid.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2169_tid05098374_00/HITS.05098374.*.pool.root.?"



Digi_tf.py --PileUpPremixing True --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ParticleGenerator_nu_E50.HITS.05232174._000001.pool.root.1 --conditionsTag default:OFLCOND-RUN12-SDR-25 --digiSeedOffset1 170 --digiSeedOffset2 170 --geometryVersion ATLAS-R2-2015-03-01-00 --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles}  --jobNumber 1 --maxEvents 25 --numberOfCavernBkg 0 --numberOfHighPtMinBias 0.12268057 --numberOfLowPtMinBias 39.8773194 --outputRDOFile ${DigiOutFileName} --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'HITtoRDO:from AthenaCommon.CfgGetter import getPublicTool;getPublicTool("MergeMcEventCollTool").OnlySaveSignalTruth=True;condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' --postInclude 'default:PyJobTransforms/UseFrontier.py' --pileupFinalBunch 6 --preExec    'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' --preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run222525_v1.py' --skipEvents 0

echo  "art-result: $? Digi_tf.py"



# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: " ${DigitizationTestsVersion}

# Do reference comparisons
art-diff.py ./$DigiOutFileName   /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
echo  "art-result: $? diff-pool"



art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=summary
echo  "art-result: $? diff-root"

checkFile ./$DigiOutFileName
echo "art-result: $? checkFile"


ArtPackage=$1
ArtJobName=$2


art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
echo  "art-result: $? art-compare"
