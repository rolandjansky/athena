#!/bin/bash
#
# art-description: Reco_tf.py q431 RAWtoESD in MT mode
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

Reco_tf.py \
--AMI q431 \
--conditionsTag 'all:CONDBR2-BLKPA-RUN2-09' \
--multithreaded="True" \
--postExec 'FPEAuditor.NStacktracesOnFPE=10' \
--maxEvents -1

rc1=$?
echo "art-result: ${rc1} Reco_tf_q431_r2e_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"
