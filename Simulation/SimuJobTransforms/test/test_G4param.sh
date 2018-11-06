#!/bin/sh
#
# art-description: MC15-style simulation using frozen showers
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation

# MC15 setup
# ATLAS-R2-2015-03-01-00 and OFLCOND-RUN12-SDR-30
export TRF_ECHO=1
AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/mu_E200_eta0-25.evgen.pool.root' \
--outputHITSFile 'mu_E200_eta0-25.HITS.pool.root' \
--maxEvents 5 \
--skipEvents 0 \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--physicsList 'FTFP_BERT_ATL' \
--preInclude 'SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--conditionsTag OFLCOND-RUN12-SDR-30 \
--DataRunNumber 222525

echo  "art-result: $? simulation"
