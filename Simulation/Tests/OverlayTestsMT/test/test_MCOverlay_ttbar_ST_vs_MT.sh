#!/bin/sh

# art-description: MC+MC Overlay with MT support, running with 8 threads
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

# art-output: MC_plus_MC.MT.RDO.pool.root
# art-output: MC_plus_MC.ST.RDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

export ATHENA_CORE_NUMBER=8

HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3681/HITS.25836812._004813.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20_13TeV.900149.PG_single_nu_Pt50.digit.RDO.e8307_s3482_s3136_d1715/RDO.26811908._031801.pool.root.1"


Overlay_tf.py \
--multithreaded \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile MC_plus_MC.MT.RDO.pool.root \
--maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-RUN2-09 \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--preInclude 'all:Campaigns/MC20e.py' \
--imf False

rc=$?
status=$rc
echo "art-result: $rc overlayMT"
mv log.Overlay log.OverlayMT

rc2=-9999
if [ $rc -eq 0 ]
then
    Overlay_tf.py \
    --inputHITSFile ${HITS_File} \
    --inputRDO_BKGFile ${RDO_BKG_File} \
    --outputRDOFile MC_plus_MC.ST.RDO.pool.root \
    --maxEvents 10 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
    --conditionsTag OFLCOND-MC16-SDR-RUN2-09 \
    --geometryVersion ATLAS-R2-2016-01-00-01 \
    --preInclude 'all:Campaigns/MC20e.py' \
    --imf False
    rc2=$?
    status=$rc2
fi
echo  "art-result: $rc2 overlayST"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root MC_plus_MC.ST.RDO.pool.root MC_plus_MC.MT.RDO.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo  "art-result: $rc3 comparison"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --order-trees --diff-root
    rc4=$?
    status=$rc4
fi
echo  "art-result: $rc4 regression"

exit $status
