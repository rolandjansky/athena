#!/bin/sh
#
# art-description: Tests ATLAS + ALFA simulation in ST and MT, generating events on-the-fly

# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-athena-mt: 8
# art-output: log.*
# art-output: test.MT.HITS.pool.root
# art-output: test.ST.HITS.pool.root

export ATHENA_CORE_NUMBER=8

AtlasG4_tf.py \
--multithreaded \
--preInclude 'ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0090.00m_nominal_v02.py,ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/EVNT.ALFA.pool.root' \
--outputHITSFile 'test.MT.HITS.pool.root' \
--maxEvents '100' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'FTFP_BERT' \
--ALFAOn 'True' \
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
    --preInclude 'ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0090.00m_nominal_v02.py,ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py' \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/EVNT.ALFA.pool.root' \
    --outputHITSFile 'test.ST.HITS.pool.root' \
    --maxEvents '100' \
    --randomSeed '10' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --DataRunNumber '222525' \
    --physicsList 'FTFP_BERT' \
    --ALFAOn 'True' \
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
