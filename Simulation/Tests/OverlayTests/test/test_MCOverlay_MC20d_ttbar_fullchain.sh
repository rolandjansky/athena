#!/bin/sh

# art-description: MC+MC Overlay chain for MC20d, ttbar, full reco chain
# art-type: grid

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

events=3
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/v1/mc20d_presampling.RDO.pool.root"

Reco_tf.py \
--autoConfiguration everything \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-08 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--maxEvents ${events} --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--preInclude "all:Campaigns/MC20d.py" \
--postInclude "default:PyJobTransforms/UseFrontier.py" \
--runNumber 410000 \
--steering "doOverlay" "doRDO_TRIG" \
--triggerConfig "RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2282,107,325" --asetup "RDOtoRDOTrigger:Athena,21.0-mc16d,latest" \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--outputAODFile MC_plus_MC.AOD.pool.root \
--imf False

rc=$?
status=$rc
echo "art-result: $rc reco"

rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries ${events} "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --file MC_plus_MC.RDO.pool.root --diff-root
    rc2=$?
    status=$rc2
fi
echo "art-result: $rc2 regression"

exit $status
