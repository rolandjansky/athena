#!/bin/sh
#
# art-description: Run digitization of an MC15 ttbar sample with 2015 geometry and conditions, without pile-up
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# the name below is needed because of the environment variable (marks storing in tar file).
# art-output: mc15_2015_ttbar_no_pileup.RDO.pool.root


DigiOutFileName="mc15_2015_ttbar_no_pileup.RDO.pool.root"


Digi_tf.py  \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/HITS.04919495._001041.pool.root.1 \
--outputRDOFile ${DigiOutFileName} \
--maxEvents 25 \
--skipEvents 0  \
--digiSeedOffset1 11 \
--digiSeedOffset2 22  \
--geometryVersion ATLAS-R2-2015-03-01-00 \
--conditionsTag default:OFLCOND-RUN12-SDR-25 \
--DataRunNumber 222500 \
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
    art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=semi-detailed
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
echo  "art-result: $rc4 art-compare"
