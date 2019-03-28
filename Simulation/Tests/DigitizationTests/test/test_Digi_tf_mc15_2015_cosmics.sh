#!/bin/sh
#
# art-description: Run digitization of a cosmics sample produced with MC15 using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: mc15_2015_cosmics.RDO.pool.root

DigiOutFileName="mc15_2015_cosmics.RDO.pool.root"

Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/testCosmics.ATLAS-R2-2015-03-01-00_VALIDATION.HITS.pool.root  --outputRDOFile ${DigiOutFileName}  --maxEvents 100  --skipEvents 0  --digiSeedOffset1 11 --digiSeedOffset2 22  --geometryVersion ATLAS-R2-2015-03-01-00_VALIDATION  --conditionsTag default:OFLCOND-RUN12-SDR-25  --DataRunNumber 222500  --postInclude 'default:PyJobTransforms/UseFrontier.py'

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
