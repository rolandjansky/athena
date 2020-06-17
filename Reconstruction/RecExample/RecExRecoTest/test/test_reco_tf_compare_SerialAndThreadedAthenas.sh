#!/bin/bash

echo "Creating new serial directory"
mkdir serial; cd serial
Reco_tf.py --AMI=$1 --preExec='RAWtoESD:from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)' --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaSerial.log
rc=${PIPESTATUS[0]}
echo "art-result: $rc Serial"

test_postProcessing_Errors.sh athenaSerial.log | tee errorsSerial.log

cd ../
echo "Creating new threadOne directory"
mkdir threadOne; cd threadOne

Reco_tf.py --athenaopts="--threads=1" --AMI=$1 --preExec='RAWtoESD:from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)' --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaOneThread.log
rc1=${PIPESTATUS[0]}
echo "art-result: $rc1 OneThread"

test_postProcessing_Errors.sh athenaOneThread.log | tee errorsOneThread.log

var=$(pwd)
echo "The current working directory $var."
ls -als

if [[ $rc -eq 0 ]] && [[ $rc1 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../serial/ | tee diffSerialOneThread.log
 echo "art-result: $? Diff-Serial-OneThread"
fi

cd ../
echo "Creating new threadTwo directory"
mkdir threadTwo; cd threadTwo

Reco_tf.py --athenaopts="--threads=2" --AMI=$1 --preExec='RAWtoESD:from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)' --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaTwoThreads.log
rc2=${PIPESTATUS[0]}
echo "art-result: $rc2 TwoThreads"

test_postProcessing_Errors.sh athenaTwoThreads.log | tee errorsTwoThreads.log

if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../threadOne | tee diffTwoThreadsOneThread.log
 echo "art-result: $? Diff-OneThread-TwoThreads"
fi

cd ../
echo "Creating new threadFive directory"
mkdir threadFive; cd threadFive

Reco_tf.py --athenaopts="--threads=5" --AMI=$1 --preExec='RAWtoESD:from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)' --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenaFiveThreads.log
rc5=${PIPESTATUS[0]}
echo "art-result: $rc5 FiveThreads"

test_postProcessing_Errors.sh athenaFiveThreads.log | tee errorsFiveThreads.log

if [[ $rc2 -eq 0 ]] && [[ $rc5 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../threadTwo | tee diffFiveThreadsTwoThreads.log
 echo "art-result: $? Diff-TwoThreads-FiveThreads"
fi
