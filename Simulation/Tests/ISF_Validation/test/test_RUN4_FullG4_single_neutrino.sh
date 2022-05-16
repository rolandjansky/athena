#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 and RUN4 geometry, single neutrino
# art-include: master/Athena
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-output: test_neutrino.HITS.pool.root

Input="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/EVNT/mc15_14TeV.900149.PG_single_nu_Pt50.evgen.EVNT.e8371/EVNT.26393337._000001.pool.root.1"
Output="test_neutrino.HITS.pool.root"

# RUN4 setup
# ATLAS-P2-RUN4-01-00-00 and OFLCOND-MC15c-SDR-14-05
Sim_tf.py \
--CA \
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-05' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms.UseFrontier' \
--preInclude 'EVNTtoHITS:Campaigns.PhaseIISimulation' \
--geometryVersion 'default:ATLAS-P2-RUN4-01-00-00' \
--inputEVNTFile "$Input" \
--outputHITSFile "$Output" \
--maxEvents 1000 \
--imf False

rc=$?
status=$rc
echo "art-result: $rc simCA"

rc2=-9999
if [ $rc -eq 0 ]; then
  art.py compare grid --entries 10 "${1}" "${2}" --mode=semi-detailed --file="$Output"
  rc2=$?
  status=$rc2
fi
echo "art-result: $rc2 regression"

exit $status
