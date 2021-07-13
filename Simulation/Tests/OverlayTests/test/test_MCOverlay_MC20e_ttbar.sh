#!/bin/sh

# art-description: MC+MC Overlay without reco for MC20e, ttbar
# art-type: grid
# art-include: master/Athena

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

events=20
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/v1/mc20e_presampling.RDO.pool.root"

Overlay_tf.py \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--maxEvents ${events} \
--skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC16-SDR-RUN2-04  \
--geometryVersion ATLAS-R2-2016-01-00-01 \
--preInclude 'all:Campaigns/MC20e.py' \
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
