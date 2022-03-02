#!/bin/sh
#
# art-description: Athena runs q431 in legacy serial mode.
# art-type: grid
# art-include: master/Athena 

preExecStringOne="RAWtoESD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False);from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)"
preExecStringTwo="ESDtoAOD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False)"
Reco_tf.py --steering "no" --AMI=q431 --preExec "${preExecStringOne}" "${preExecStringTwo}" --conditionsTag="CONDBR2-BLKPA-RUN2-09" --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaSerial.log
echo "art-result: ${PIPESTATUS[0]}"
test_postProcessing_Errors.sh athenaSerial.log | tee errorsSerial.log
