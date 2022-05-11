#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 and RUN4 geometry, single muon
# art-include: master/Athena
# art-type: grid
# art-output: test_muons.HITS.pool.root

Input=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/EVNT/mc15_14TeV.422036.ParticleGun_single_mu_Pt100.evgen.EVNT.e5286/EVNT.09244578._000001.pool.root.1

# RUN4 setup
# ATLAS-P2-RUN4-01-00-00 and OFLCOND-MC15c-SDR-14-05
Sim_tf.py \
--CA \
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-05' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms.UseFrontier' \
--preExec 'ConfigFlags.HGTD.Geometry.useGeoModelXml = True;' \
--preInclude 'EVNTtoHITS:Campaigns.PhaseIISimulation' \
--geometryVersion 'default:ATLAS-P2-RUN4-01-00-00' \
--inputEVNTFile $Input \
--outputHITSFile "test_muons.HITS.pool.root" \
--maxEvents 1 \
--imf False

rc=$?
echo "art-result: $rc simCA"

exit $rc
