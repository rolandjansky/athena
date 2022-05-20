#!/bin/sh

# art-description: MC+MC Overlay without reco for MC21a, ttbar
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-include: 22.0/Athena
# art-include: master/Athena

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

events=20
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc21_13p6TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.simul.HITS.e8357_e7400_s3775/HITS.27679639._074687.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run3/v3/mc21a_presampling.RDO.pool.root"

Overlay_tf.py \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--maxEvents ${events} \
--skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC21-SDR-RUN3-05  \
--geometryVersion ATLAS-R3S-2021-02-00-00 \
--preInclude 'all:Campaigns/MC21a.py' \
--imf False

rc=$?
status=$rc
echo "art-result: $rc overlay"

rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --file MC_plus_MC.RDO.pool.root --diff-root
    rc2=$?
    status=$rc2
fi
echo "art-result: $rc2 regression"

exit $status
