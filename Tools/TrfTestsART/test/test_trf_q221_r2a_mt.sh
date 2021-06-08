#!/bin/bash
#
# art-description: Reco_tf.py q431 HITStoRDO and RAWtoALL/TRIGtoALL in MT mode, RDOtoRDOTrigger in 21.0.126
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

Reco_tf.py --multithreaded='True' --AMI=q221 --steering "doRDO_TRIG" "doTRIGtoALL" --triggerConfig "RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2233,87,314" --asetup "RDOtoRDOTrigger:Athena,21.0.126" --imf="False" --maxEvents 1000

echo "art-result: $? Reco_tf_q221_r2a_mt"
