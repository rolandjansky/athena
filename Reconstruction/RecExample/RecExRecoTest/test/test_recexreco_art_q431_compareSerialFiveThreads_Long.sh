#!/bin/sh
#
# art-description: Runs q431 in serial and five thread modes with 1000 events, then runs diff-root.
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: runOne
# art-output: runTwo
# art-runon: Monday

preExecStringOne="RAWtoESD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False);from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)"
preExecStringTwo="ESDtoAOD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False)"

mkdir runOne; cd runOne
Reco_tf.py --athenaopts="--threads=5" --maxEvents=1000 --steering "no" --AMI=q431 --preExec "${preExecStringOne}" "${preExecStringTwo}" --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenarunOne.log
rc1=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestOne.log
echo "art-result: $rc1 runOne"

cd ../
mkdir runTwo; cd runTwo
Reco_tf.py --athenaopts="--threads=5" --maxEvents=1000 --steering "no" --AMI=q431 --preExec "${preExecStringOne}" "${preExecStringTwo}" --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenarunTwo.log
rc2=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestTwo.log
echo "art-result: $rc2 runTwo"

if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../runOne/ | tee diffFiveThreads.log
 rcDiff=${PIPESTATUS[0]}
 collateDigest.sh digestTwo.log ../runOne/digestOne.log digestDiffOneTwo.log 
 echo "art-result: $rcDiff Diff-Serial-OneThread"
fi



