#!/bin/bash
#
# art-description: Reco_tf.py q431 RAWtoALL in MT mode
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

Reco_tf.py --AMI q431 --multithreaded="True" --steering doRAWtoALL --maxEvents -1

echo "art-result: $? Reco_tf_q431_r2a_mt"
