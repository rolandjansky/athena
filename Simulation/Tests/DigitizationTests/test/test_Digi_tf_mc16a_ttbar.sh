#!/bin/bash
#
# art-description: Run digitization of an MC16a ttbar sample with 2016a geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-output: mc16a_ttbar.RDO.pool.root

DigiOutFileName="mc16a_ttbar.RDO.pool.root"

InputHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


Digi_tf.py \
--inputHITSFile ${InputHitsFile} \
--conditionsTag default:OFLCOND-MC16-SDR-25-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 25 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'all:Campaigns/MC16a.py' 'HITtoRDO:Campaigns/PileUpMC16a.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py"
rc1=-9999
rc2=-9999
rc3=-9999
rc4=-9999

# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: ${DigitizationTestsVersion}"

if [[ $rc -eq 0 ]]
then
    # Do reference comparisons
    art.py compare ref --diff-pool "$DigiOutFileName" "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName"
    rc1=$?
    status=$rc1
fi
echo "art-result: $rc1 diff-pool"
#
#
#
if [[ $rc -eq 0 ]]
then
    art.py compare ref --mode=semi-detailed --diff-root "$DigiOutFileName" "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName"
    rc2=$?
    status=$rc2
fi
echo "art-result: $rc2 diff-root"
#
if [[ $rc -eq 0 ]]
then
    checkFile "$DigiOutFileName"
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 checkFile"
#
#
if [[ $rc -eq 0 ]]
then
    art.py compare grid --entries 10 "$1" "$2" --mode=semi-detailed
    rc4=$?
    status=$rc4
fi
echo "art-result: $rc4 regression"

exit $status
