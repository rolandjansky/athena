#!/bin/sh
#
# art-description: Run MC16d pile-up pre-mixing with 2016d geometry and conditions, 25ns pile-up, MT output containers
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-output: mc16d_premixing_MT.RDO.pool.root

DigiOutFileName="mc16d_premixing_MT.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


Digi_tf.py \
--PileUpPremixing True \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc16_valid.422075.ParticleGun_single_nu_Pt50.simul.HITS.e6739_e5984_s3126.HITS.14436922._003949.pool.root \
--conditionsTag default:OFLCOND-MC16-SDR-20 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 25 \
--numberOfCavernBkg 0 \
--numberOfHighPtMinBias 0.2099789464 \
--numberOfLowPtMinBias 80.290021063135 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf "StandardSignalOnlyTruth" \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'HITtoRDO:condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--pileupFinalBunch 6 \
--preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)' \
'all:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.experimentalDigi += ["OverlayMT"];' \
--preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configEvtNbr_sequential.py,RunDependentSimData/configLumi_run300000_mc16d.py' \
--skipEvents 0

rc=$?
echo  "art-result: $rc Digi_tf.py"
rc1=-9999
rc2=-9999
rc3=-9999
rc4=-9999

# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: " ${DigitizationTestsVersion}

if [ $rc -eq 0 ]
then
    # Do reference comparisons
    art.py compare ref --diff-pool $DigiOutFileName   /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
    rc1=$?
fi
echo  "art-result: $rc1 diff-pool"
#
#
#
if [ $rc -eq 0 ]
then
    art.py compare ref --mode=semi-detailed --diff-root $DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
    rc2=$?
fi
echo  "art-result: $rc2 diff-root"
#
if [ $rc -eq 0 ]
then
    checkFile ./$DigiOutFileName
    rc3=$?
fi
echo "art-result: $rc3 checkFile"
#
#
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc4=$?
fi
echo  "art-result: $rc4 regression"
