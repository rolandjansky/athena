#!/bin/bash
#
# art-description: Reco_tf.py q431 RAWtoESD in MT mode
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

Reco_tf.py \
--AMI q431 \
--conditionsTag 'all:CONDBR2-BLKPA-RUN2-07' \
--multithreaded="True" \
--maxEvents -1

echo "art-result: $? Reco_tf_q431_r2e_mt"
