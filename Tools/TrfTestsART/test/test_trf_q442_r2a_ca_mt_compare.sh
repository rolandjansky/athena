#!/bin/bash
#
# art-description: Reco_tf.py q442 RAWtoALL in MT and ComponentAccumulator mode
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

mkdir ca
cd ca
Reco_tf.py --CA \
  --AMI q442  \
  --multithreaded="True" \
  --outputAODFile myAOD_ca.pool.root \
  --outputESDFile myESD_ca.pool.root \
  --maxEvents 100

rc1=$?
echo "art-result: ${rc1} Reco_tf_q442_r2a_ca_mt" 

cd ..
mkdir def
cd def
Reco_tf.py \
  --AMI q442  \
  --multithreaded="True" \
  --outputAODFile myAOD_def.pool.root \
  --outputESDFile myESD_def.pool.root \
  --maxEvents 100

rc2=$?
echo "art-result: ${rc2} Reco_tf_q442_r2a_mt" 

cd ..

echo "============ checkxAOD myAOD_ca.pool.root"
checkxAOD ca/myAOD_ca.pool.root
rc3=$?
echo "art-result: ${rc3} checkxAOD myAOD_ca.pool.root" 

echo "============ checkxAOD myAOD_def.pool.root"
checkxAOD def/myAOD_def.pool.root
rc4=$?
echo "art-result: ${rc4} checkxAOD myAOD_def.pool.root" 

echo "============ xAODDigest.py myAOD_ca.pool.root"
xAODDigest.py ca/myAOD_ca.pool.root myAOD_ca.txt
rc5=$?
echo "art-result: ${rc5} xAODDigest.py myAOD_ca.pool.root" 
echo "============ myAOD_ca.txt"
cat myAOD_ca.txt
echo "============ myAOD_ca.txt"

echo "============ xAODDigest.py myAOD_def.pool.root"
xAODDigest.py def/myAOD_def.pool.root myAOD_def.txt
rc6=$?
echo "art-result: ${rc6} xAODDigest.py myAOD_def.pool.root" 
echo "============ myAOD_def.txt"
cat myAOD_def.txt
echo "============ myAOD_def.txt"

echo "============ comparexAODDigest.py myAOD_ca.txt myAOD_def.txt"
comparexAODDigest.py myAOD_ca.txt myAOD_def.txt
rc7=$?
echo "art-result: ${rc7} comparexAODDigest.py myAOD_ca.txt myAOD_def.txt" 
