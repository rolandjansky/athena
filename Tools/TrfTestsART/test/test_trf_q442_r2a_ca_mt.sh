#!/bin/bash
#
# art-description: Reco_tf.py q442 RAWtoALL in MT and ComponentAccumulator mode
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

Reco_tf.py --CA \
  --AMI q442  \
  --multithreaded="True" \
  --outputAODFile myAOD.pool.root \
  --outputESDFile myESD.pool.root \
  --maxEvents -1

rc1=$?
echo "art-result: ${rc1} Reco_tf_q442_r2a_ca_mt" 

# Check for FPEs in the logiles
# test_trf_check_fpe.sh      # currently disabled since FPEAuditor does not work in CA mode
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"

echo "============ checkxAOD myAOD.pool.root"
checkxAOD myAOD.pool.root
rc2=$?
echo "art-result: ${rc2} checkxAOD" 
