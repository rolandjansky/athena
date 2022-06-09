#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 and RUN4 geometry, ttbar
# art-include: master/Athena
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-output: test.HITS.pool.root
# art-output: truth.root

Input="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1"
Output="test.HITS.pool.root"

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
--maxEvents 5 \
--imf False

rc=$?
status=$rc
echo "art-result: $rc simCA"

rc2=-9999
if [ $rc -eq 0 ]; then
  art.py compare grid --entries 5 "${1}" "${2}" --mode=semi-detailed --file="$Output"
  rc2=$?
  status=$rc2
fi
echo "art-result: $rc2 regression"

exit $status
