#!/bin/bash
#
# art-description: Run 4 digitization of a single muon sample without pile-up
# art-type: grid
# art-include: master/Athena
# art-output: RUN4_muons.RDO.pool.root

Events=1000
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/single_muon.HITS.pool.root"
DigiOutFileName="RUN4_muons.RDO.pool.root"

Digi_tf.py \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-P2-ITK-24-00-00 \
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

exit $status
