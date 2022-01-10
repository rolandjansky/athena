#!/bin/sh
#
# art-description: MC21-style simulation using FullG4_QS (13 TeV Zprime input - needs updating)
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.HITS.pool.root

# RUN3 setup - Frozen Showers currently off by default
# ATLAS-R3S-2021-02-00-00 and OFLCOND-MC21-SDR-RUN3-03
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4_QS' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC21Simulation.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/EVNT.04607198._000001.pool.root.1" \
--outputHITSFile "test.HITS.pool.root" \
--maxEvents 10 \
--imf False

rc=$?
rc2=-9999
echo  "art-result: $rc simulation"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc2=$?
fi

echo  "art-result: $rc2 regression"
