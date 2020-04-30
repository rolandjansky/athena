#!/bin/sh
#
# art-description: Run cosmics simulation using ISF with the FullG4 simulator, generating events on-the-fly, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: test.TR.pool.root

Sim_tf.py \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'QGSP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--outputEVNT_TRFile 'test.TR.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '1500' \
--randomSeed '1234' \
--DataRunNumber '10' \
--CosmicFilterVolume 'Calo' \
--CosmicFilterVolume2 'NONE' \
--preInclude 'SimulationJobOptions/preInclude.Cosmics.py' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--CosmicPtSlice 'NONE' \
--beamType 'cosmics' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid ${ArtPackage} ${ArtJobName} --mode=semi-detailed

echo  "art-result: $? regression"
