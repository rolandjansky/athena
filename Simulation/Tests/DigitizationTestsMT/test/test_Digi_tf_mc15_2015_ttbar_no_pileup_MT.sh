#!/bin/sh
#
# art-description: Run digitization of an MC15 ttbar sample with 2015 geometry and conditions, without pile-up using two threads
# art-type: grid
# art-athena-mt: 2
# art-include: master/Athena
# the name below is needed because of the environment variable (marks storing in tar file).
# art-output: mc15_2015_ttbar_no_pileup.RDO.pool.root
# art-output: log.*

Digi_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/HITS.04919495._001041.pool.root.1 \
--conditionsTag default:OFLCOND-RUN12-SDR-25 \
--digiSeedOffset1 170 \
--digiSeedOffset2 170 \
--geometryVersion ATLAS-R2-2015-03-01-00 \
--DataRunNumber 222525 \
--outputRDOFile mc15_2015_ttbar.RDO.pool.root \
--preInclude 'HITtoRDO:Digitization/ForceUseOfAlgorithms.py' \
--postInclude 'Digitization/FixDataDependenciesForMT.py' \
--skipEvents 0  \
--maxEvents 10 \
--athenaopts '--threads 2'

echo  "art-result: $? Digi_tf.py"

#ArtPackage=$1
#ArtJobName=$2
#
#art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
#echo  "art-result: $? art-compare"
