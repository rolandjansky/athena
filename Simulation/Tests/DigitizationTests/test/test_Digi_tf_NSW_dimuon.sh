#!/bin/sh
#
# art-description: Run digitization of a dimuon sample with Run 3 symmetric geometry, 25ns pile-up
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-output: NSW_dimuon.RDO.pool.root

DigiOutFileName="NSW_dimuon.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.high-pTMinBias.rel_master_2021_01_27.merge.HITS_EXT0/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.low-pTMinBias.rel_master_2021_01_27.merge.HITS_EXT0/*"


Digi_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.DiMuon10_100GeV.HITS.rel_master_2020_12_01_R3S_v01_EXT1/group.det-muon.23437494.EXT1._000011.HITS.pool.root \
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
--preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)' \
--preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_mc16a.py' \
--skipEvents 0

rc=$?
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
# fi
# echo  "art-result: $rc1 diff-pool"
# #
# #
# #
# if [ $rc -eq 0 ]
# then
#     art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=semi-detailed
#     rc2=$?
# fi
# echo  "art-result: $rc2 diff-root"
# #
# if [ $rc -eq 0 ]
# then
#     checkFile ./$DigiOutFileName
#     rc3=$?
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
# fi
# echo  "art-result: $rc4 art-compare"
