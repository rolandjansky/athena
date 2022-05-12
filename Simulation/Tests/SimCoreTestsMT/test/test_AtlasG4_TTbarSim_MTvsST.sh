#!/bin/sh
#
# art-description: Run MT and ST simulation outside ISF, reading ttbar events, writing HITS, using MC16 geometry and conditions
# art-include: master/Athena

# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-athena-mt: 8
# art-output: log.*
# art-output: test.MT.HITS.pool.root
# art-output: test.ST.HITS.pool.root

export ATHENA_CORE_NUMBER=8

AtlasG4_tf.py \
--multithreaded \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--outputHITSFile 'test.MT.HITS.pool.root' \
--maxEvents '10' \
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
echo  "art-result: $rc MTsim"
rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    unset ATHENA_CORE_NUMBER
    AtlasG4_tf.py \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
    --outputHITSFile 'test.ST.HITS.pool.root' \
    --maxEvents '10' \
    --skipEvents '0' \
    --randomSeed '10' \
    --geometryVersion 'ATLAS-R2-2016-01-00-01' \
    --conditionsTag 'OFLCOND-MC16-SDR-14' \
    --DataRunNumber '284500' \
    --physicsList 'FTFP_BERT_ATL' \
    --postInclude 'PyJobTransforms/UseFrontier.py' \
    --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
    --imf False
    rc2=$?
fi
echo  "art-result: $rc2 STsim"
rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.MT.HITS.pool.root test.ST.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 comparision"
