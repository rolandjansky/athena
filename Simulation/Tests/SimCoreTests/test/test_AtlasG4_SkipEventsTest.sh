#!/bin/sh
#
# art-description: Reading single particle gen events, checking that the SkipEvents argument works, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.HITS.pool.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/e_E50_eta0-25.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '10' \
--skipEvents '5' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO Here we also want to be able to grep the log file for:
#<fileGrepperPattern>skipping event 1.*skipping event 2.*skipping event 3.*skipping event 4.*skipping event 5</fileGrepperPattern>
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed

echo  "art-result: $? regression"
