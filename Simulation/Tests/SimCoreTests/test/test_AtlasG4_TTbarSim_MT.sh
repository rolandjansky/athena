#!/bin/sh
#
# art-description: Run MT simulation outside ISF, reading ttbar events, writing HITS, using MC16 geometry and conditions
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '20' \
--skipEvents '0' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2016-01-00-01' \
--conditionsTag 'OFLCOND-MC16-SDR-14' \
--DataRunNumber '284500' \
--physicsList 'FTFP_BERT_ATL' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
--athenaopts '--threads 6' \
--imf False

echo  "art-result: $? simulation"
