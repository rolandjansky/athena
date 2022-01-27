#!/bin/bash
#
# art-description: Reco_tf.py q431, RAWtoALL in MT and AODtoDAOD in MP
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

export ATHENA_CORE_NUMBER=8
Reco_tf.py \
  --AMI q431 \
  --conditionsTag 'all:CONDBR2-BLKPA-RUN2-09' \
  --sharedWriter True \
  --steering doRAWtoALL \
  --outputDAODFile art.pool.root \
  --reductionConf PHYS PHYSLITE \
  --athenaopts "RAWtoALL:--threads=${ATHENA_CORE_NUMBER} --nprocs=0" "AODtoDAOD:--threads=0 --nprocs=${ATHENA_CORE_NUMBER}" \
  --postExec 'FPEAuditor.NStacktracesOnFPE=10' \
  --maxEvents -1

rc1=$?
echo "art-result: ${rc1} Reco_tf_q431_phys_physlite_mt_mp" 

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"

echo "============ checkxAOD DAOD_PHYS.art.pool.root"
checkxAOD DAOD_PHYS.art.pool.root
echo "============ checkxAOD DAOD_PHYSLITE.art.pool.root"
checkxAOD DAOD_PHYSLITE.art.pool.root
rc2=$?
echo "art-result: ${rc2} checkxAOD" 
