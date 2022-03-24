#!/bin/bash
#
# art-description: Reco_tf.py q221, HITtoRDO/RDOtoRDOTrigger/RAWtoALL in MT and AODtoDAOD in MP
# art-type: grid
# art-athena-mt: 8

export ATHENA_CORE_NUMBER=8
Reco_tf.py \
  --AMI q221 \
  --conditionsTag 'all:OFLCOND-MC16-SDR-RUN2-09' \
  --sharedWriter True \
  --steering 'doRDO_TRIG' 'doTRIGtoALL' \
  --outputDAODFile art.pool.root \
  --reductionConf PHYS PHYSLITE \
  --athenaopts "HITtoRDO:--threads=${ATHENA_CORE_NUMBER} --nprocs=0" "RDOtoRDOTrigger:--threads=${ATHENA_CORE_NUMBER} --nprocs=0" "RAWtoALL:--threads=${ATHENA_CORE_NUMBER} --nprocs=0" "AODtoDAOD:--threads=0 --nprocs=${ATHENA_CORE_NUMBER}" \
  --postExec "from AthenaAuditors.AthenaAuditorsConf import FPEAuditor;FPEAuditor.NStacktracesOnFPE=10" \
  --maxEvents -1

rc1=$?
echo "art-result: ${rc1} Reco_tf_q221_phys_physlite_mt_mp" 

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
