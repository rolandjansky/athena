#!/bin/bash
#
# art-description: Run 4 digitization of a ttbar sample without pile-up
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-include: master/Athena
# art-output: RUN4_ttbar.RDO.pool.root

Events=25
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-00-00/mc15_14TeV.600012.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e8185_s3856/HITS.29179779._000100.pool.root.1"
DigiOutFileName="RUN4_ttbar.RDO.pool.root"

Digi_tf.py \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-P2-RUN4-01-00-00 \
--inputHITSFile ${HSHitsFile} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileName} \
--preInclude 'HITtoRDO:Campaigns.PhaseIINoPileUp' \
--postInclude 'PyJobTransforms.UseFrontier' \
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
