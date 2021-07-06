#!/bin/bash

preExecStringOne="RAWtoESD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False);from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)"
preExecStringTwo="ESDtoAOD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False)"

echo "Creating new serial directory"
mkdir serial; cd serial
Reco_tf.py --steering "no" --AMI=$1 --preExec "${preExecStringOne}" "${preExecStringTwo}" --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaSerial.log
rc=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestSerial.log
echo "art-result: $rc Serial"

test_postProcessing_Errors.sh athenaSerial.log | tee errorsSerial.log

cd ../
echo "Creating new threadOne directory"
mkdir threadOne; cd threadOne

Reco_tf.py --steering "no" --athenaopts="--threads=1" --AMI=$1 --preExec "${preExecStringOne}" "${preExecStringTwo}"  --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaOneThread.log
rc1=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestOneThread.log
echo "art-result: $rc1 OneThread"

test_postProcessing_Errors.sh athenaOneThread.log | tee errorsOneThread.log

var=$(pwd)
echo "The current working directory $var."
ls -als

if [[ $rc -eq 0 ]] && [[ $rc1 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../serial/ | tee diffSerialOneThread.log
 rcDiff1=${PIPESTATUS[0]}
 collateDigest.sh digestOneThread.log ../serial/digestSerial.log digestDiffSerialOneThread.log 
 echo "art-result: $rcDiff1 Diff-Serial-OneThread"
fi

cd ../
echo "Creating new threadEight directory"
mkdir threadEight; cd threadEight

Reco_tf.py --steering "no" --athenaopts="--threads=8" --AMI=$1 --preExec "${preExecStringOne}" "${preExecStringTwo}"  --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaEightThreads.log
rc2=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestEightThreads.log
echo "art-result: $rc2 EightThreads"

test_postProcessing_Errors.sh athenaEightThreads.log | tee errorsEightThreads.log

if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../threadOne | tee diffEightThreadsOneThread.log
 rcDiff2=${PIPESTATUS[0]}
 collateDigest.sh digestEightThreads.log ../threadOne/digestOneThread.log digestDiffEightThreadsOneThread.log 
 echo "art-result: $rcDiff2 Diff-OneThread-EightThreads"
fi
