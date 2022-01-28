#!/bin/bash
#
# art-description: Run digitization of an MC20 ttbar sample with 2018 geometry and conditions, without pile-up using AthenaMT with one thread
# art-type: grid
# art-athena-mt: 8
# art-include: 22.0-mc20/Athena
# art-include: master/Athena
# the name below is needed because of the environment variable (marks storing in tar file).
# art-output: mc20_nopileup_ttbar.RDO.pool.root
# art-output: log.*

export ATHENA_CORE_NUMBER=1

Digi_tf.py \
--multithreaded \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
--digiSeedOffset1 170 \
--digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--DataRunNumber 310000 \
--outputRDOFile mc20_nopileup_ttbar.RDO.pool.root \
--preInclude 'HITtoRDO:Campaigns/MC20NoPileUp.py,Digitization/ForceUseOfAlgorithms.py' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--skipEvents 0  \
--maxEvents 10

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py"

rc2=-9999
if [[ $rc -eq 0 ]]
then
    art.py compare grid --entries 10 "$1" "$2" --mode=semi-detailed
    rc2=$?
    status=$rc2
fi
echo "art-result: $rc2 regression"

exit $status
