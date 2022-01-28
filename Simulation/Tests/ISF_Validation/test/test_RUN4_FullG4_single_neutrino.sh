#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 and RUN4 geometry, single neutrino
# art-include: master/Athena
# art-type: grid
# art-output: test_neutrino.HITS.pool.root

Input=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/EVNT/mc15_14TeV.900149.PG_single_nu_Pt50.evgen.EVNT.e8371/EVNT.26393337._000001.pool.root.1

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
--outputHITSFile "test_neutrino.HITS.pool.root" \
--maxEvents 1000 \
--imf False

rc=$?
echo "art-result: $rc simCA"

exit $rc
