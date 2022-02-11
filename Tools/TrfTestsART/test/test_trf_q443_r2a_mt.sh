#!/bin/bash
#
# art-description: Reco_tf.py q443 HITStoRDO,RAWtoALL/TRIGtoALL in MT mode, RDOtoRDOTrigger in 21.0/MP
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

Reco_tf.py \
  --multithreaded='True' \
  --AMI=q443 \
  --steering "doRDO_TRIG" "doTRIGtoALL" \
  --imf="False" \
  --maxEvents 1000

rc1=$?
echo "art-result: ${rc1} Reco_tf_q443_r2a_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"
