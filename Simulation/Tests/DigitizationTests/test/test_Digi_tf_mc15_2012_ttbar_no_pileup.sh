#!/bin/sh
#
# art-description: Run digitization of an MC15 ttbar sample with 2012 geometry and conditions, without pile-up
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: mc15_2012_ttbar_no_pileup.RDO.pool.root

DigiOutFileName="mc15_2012_ttbar_no_pileup.RDO.pool.root"



Digi_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ttbar.ATLAS-R1-2012-03-00-00.HITS.pool.root \
--outputRDOFile ${DigiOutFileName}  \
--maxEvents 25 \
--skipEvents 0  \
--digiSeedOffset1=11 \
--digiSeedOffset2=22 \
--geometryVersion ATLAS-R1-2012-03-00-00 \
--conditionsTag OFLCOND-RUN12-SDR-22 \
--DataRunNumber 212272 \
--postInclude 'default:PyJobTransforms/UseFrontier.py'

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
    art-diff.py ./$DigiOutFileName   /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
    rc1=$?
fi
echo  "art-result: $rc1 diff-pool"
#
#
#
if [ $rc -eq 0 ]
then
    art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=summary
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
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc4=$?
fi
echo  "art-result: $rc4 art-compare"
