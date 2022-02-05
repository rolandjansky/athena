#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 and RUN4 geometry, ttbar
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

Input=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1

# RUN4 setup
# ATLAS-P2-ITK-24-00-00 and OFLCOND-MC15c-SDR-14-05
Sim_tf.py \
--CA \
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-05' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms.UseFrontier' \
--preInclude 'EVNTtoHITS:Campaigns.PhaseIISimulation' \
--geometryVersion 'default:ATLAS-P2-ITK-24-00-00' \
--inputEVNTFile $Input \
--outputHITSFile "test.HITS.pool.root" \
--maxEvents 5 \
--imf False

rc=$?
echo "art-result: $rc simCA"

exit $rc
