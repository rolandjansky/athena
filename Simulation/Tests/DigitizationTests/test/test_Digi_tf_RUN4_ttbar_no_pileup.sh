#!/bin/bash
#
# art-description: Run 4 digitization of a ttbar sample without pile-up
# art-type: grid
# art-include: master/Athena
# art-output: RUN4_ttbar.RDO.pool.root

Events=5
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ttbar.HITS.pool.root"
DigiOutFileName="RUN4_ttbar.RDO.pool.root"

Digi_tf.py \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-P2-ITK-24-00-00 \
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

exit $status
