#!/bin/sh
#
# art-description: Run MT simulation outside ISF, reading ttbar events, writing HITS, using MC16 geometry and conditions
# art-include: master/Athena

# art-type: grid
# art-athena-mt: 4
# art-output: ordered.HITS.pool.root
# art-output: log.*
# art-output: tree-orderer.log

AtlasG4_tf.py \
--athenaopts '--threads 4' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--outputHITSFile 'unordered.HITS.pool.root' \
--maxEvents '20' \
--skipEvents '0' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2016-01-00-01' \
--conditionsTag 'OFLCOND-MC16-SDR-14' \
--DataRunNumber '284500' \
--physicsList 'FTFP_BERT_ATL' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
--imf False

rc=$?
echo  "art-result: $rc simulation"
rc2=-9999
if [ $rc -eq 0 ]
then
    tree-orderer.py -i unordered.HITS.pool.root -o ordered.HITS.pool.root
    rc2=$?
    rm unordered.HITS.pool.root
fi
echo  "art-result: $rc2 reordering"
rc3=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc3=$?
fi
echo  "art-result: $rc3 regression"
