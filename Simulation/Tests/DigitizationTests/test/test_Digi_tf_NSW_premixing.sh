#!/bin/sh
#
# art-description: Run NSW pile-up pre-mixing with Run 3 symmetric geometry, 25ns pile-up
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-output: NSW_premixing.RDO.pool.root

DigiOutFileName="NSW_premixing.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.high-pTMinBias.rel_master_2021_01_27.merge.HITS_EXT0/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.low-pTMinBias.rel_master_2021_01_27.merge.HITS_EXT0/*"


Digi_tf.py \
--PileUpPresampling True \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/Neutrinos_R3S_v01.pool.root \
--conditionsTag default:OFLCOND-MC16-SDR-14 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R3S-2021-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 25 \
--numberOfCavernBkg 0 \
--numberOfHighPtMinBias 0.116075313 \
--numberOfLowPtMinBias 44.3839246425 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf "StandardSignalOnlyTruth" \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'HITtoRDO:condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--pileupFinalBunch 6 \
--preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);' \
'all:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.experimentalDigi += ["LegacyOverlay"];' \
--preInclude 'all:LArConfiguration/LArConfigRun2Old.py' 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configEvtNbr_sequential.py,RunDependentSimData/configLumi_run284500_mc16a.py' \
--skipEvents 0

rc=$?
status=$rc
echo  "art-result: $rc Digi_tf.py"
# rc1=-9999
# rc2=-9999
# rc3=-9999
# rc4=-9999

# # get reference directory
# source DigitizationCheckReferenceLocation.sh
# echo "Reference set being used: " ${DigitizationTestsVersion}

# if [ $rc -eq 0 ]
# then
#     # Do reference comparisons
#     art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
#     rc1=$?
#     status=$rc1
# fi
# echo  "art-result: $rc1 diff-pool"
# #
# #
# #
# if [ $rc -eq 0 ]
# then
#     art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=semi-detailed
#     rc2=$?
#     status=$rc2
# fi
# echo  "art-result: $rc2 diff-root"
# #
# if [ $rc -eq 0 ]
# then
#     checkFile ./$DigiOutFileName
#     rc3=$?
#     status=$rc3
# fi
# echo "art-result: $rc3 checkFile"
# #
# #
# if [ $rc -eq 0 ]
# then
#     ArtPackage=$1
#     ArtJobName=$2
#     art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
#     rc4=$?
#     status=$rc4
# fi
# echo  "art-result: $rc4 art-compare"

exit $status
