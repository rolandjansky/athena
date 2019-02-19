#!/bin/sh
#
# art-description: Run digitization of an mc16d ttbar sample with 2016d geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-output: mc16d_ttbar.RDO.pool.root

DigiOutFileName="mc16d_ttbar.RDO.pool.root"




HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"



#--digiSteeringConf "StandardSignalOnlyTruth"

Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 --conditionsTag default:OFLCOND-MC16-SDR-20 --digiSeedOffset1 170 --digiSeedOffset2 170 --geometryVersion default:ATLAS-R2-2016-01-00-01 --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} --jobNumber 1 --maxEvents 25 --numberOfCavernBkg 0 --numberOfHighPtMinBias 0.2099789464 --numberOfLowPtMinBias 80.290021063135 --outputRDOFile ${DigiOutFileName} --digiSteeringConf "StandardSignalOnlyTruth" --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' --postInclude 'default:PyJobTransforms/UseFrontier.py' --pileupFinalBunch 6 --preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)' --preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run300000_mc16d.py' --skipEvents 0


echo  "art-result: $? Digi_tf.py"


# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: " ${DigitizationTestsVersion}

# Do reference comparisons
art-diff.py ./$DigiOutFileName   /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
echo  "art-result: $? diff-pool"
#
#
#
art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=summary
echo  "art-result: $? diff-root"
#
checkFile ./$DigiOutFileName
echo "art-result: $? checkFile"
#
#
ArtPackage=$1
ArtJobName=$2
#
#
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
echo  "art-result: $? art-compare"
