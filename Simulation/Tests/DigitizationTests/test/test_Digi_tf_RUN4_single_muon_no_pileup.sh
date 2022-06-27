#!/bin/bash
#
# art-description: Run 4 digitization of a single muon sample without pile-up
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-include: master/Athena
# art-output: RUN4_muons.RDO.pool.root

Events=1000
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-00-00/mc15_14TeV.900040.PG_singlemu_Pt100_etaFlatnp0_43.simul.HITS.e8185_s3856/HITS.29179781._000024.pool.root.1"
DigiOutFileName="RUN4_muons.RDO.pool.root"

Digi_tf.py \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-P2-RUN4-01-00-00 \
--inputHITSFile ${HSHitsFile} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileName} \
--postInclude 'PyJobTransforms.UseFrontier' \
--preInclude 'HITtoRDO:Campaigns.PhaseIINoPileUp' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc digiCA"

rc2=-9999
if [ $rc -eq 0 ]; then
  art.py compare grid --entries 10 "${1}" "${2}" --mode=semi-detailed --file="$DigiOutFileName"
  rc2=$?
  status=$rc2
fi
echo "art-result: $rc2 regression"

exit $status
