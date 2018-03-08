#!/bin/sh
#
# art-description: Run digitization of an MC15 ttbar sample with 2015 geometry and conditions, without pile-up
# art-type: grid


DigiOutFileName="mc15_2015_ttbar_no_pileup.RDO.pool.root"

Digi_tf.py  --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/HITS.04919495._001041.pool.root.1 --outputRDOFile ${DigiOutFileName} --maxEvents 25 --skipEvents 0  --digiSeedOffset1 11 --digiSeedOffset2 22  --geometryVersion ATLAS-R2-2015-03-01-00 --conditionsTag default:OFLCOND-RUN12-SDR-25 --DataRunNumber 222500 --postInclude 'default:PyJobTransforms/UseFrontier.py'


echo  "art-result: $? Digi_tf.py"

# get reference directory 
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: " ${DigitizationTestsVersion} 

# Do reference comparisons 
art-diff.py ./$DigiOutFileName /eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
echo  "art-result: $? diff-pool" 



art-diff.py ./$DigiOutFileName /eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root
echo  "art-result: $? diff-root" 


ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests


art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo  "art-result: $? art-compare" 
