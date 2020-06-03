#!/bin/sh
#
# art-description: Create a minbias HITS file then run FilterHit_tf.py on it
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

# MC15 setup
# ATLAS-R2-2015-03-01-00 and OFLCOND-RUN12-SDR-30
export TRF_ECHO=1
AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic-pythia8-7000.evgen.pool.root' \
--outputHITSFile 'minbias_Inelastic-pythia8-7000_ATLAS-R2-2015-03-01-00.HITS.pool.root' \
--maxEvents 5 \
--skipEvents 0 \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--physicsList 'FTFP_BERT_ATL' \
--preInclude 'SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--conditionsTag OFLCOND-RUN12-SDR-30 \
--DataRunNumber 222525

echo  "art-result: $? simulation"

FilterHit_tf.py \
 --inputHITSFile 'minbias_Inelastic-pythia8-7000_ATLAS-R2-2015-03-01-00.HITS.pool.root' \
 --outputHITS_FILTFile 'minbias_Inelastic-pythia8-7000_ATLAS-R2-2015-03-01-00.filtered.HITS.pool.root'\
 --maxEvents 5 \
 --skipEvents 0 \
 --geometryVersion 'ATLAS-R2-2015-03-01-00'

echo  "art-result: $? filter"
