#!/bin/sh
#
# art-description: Athena runs q431 with 1 and 8 threads twice, and then does diff-root.
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: runOne
# art-output: runTwo

preExecStringOne="RAWtoESD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False);from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)"
preExecStringTwo="ESDtoAOD:from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False)"

mkdir runOne; cd runOne
Reco_tf.py --athenaopts="--threads=1" --steering "no" --AMI=q431 --preExec "${preExecStringOne}" "${preExecStringTwo}" --conditionsTag="CONDBR2-BLKPA-RUN2-09" --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenarunOne.log
rc1=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestOne.log
echo "art-result: $rc1 runOne"

cd ../
mkdir runTwo; cd runTwo
Reco_tf.py --athenaopts="--threads=8" --steering "no" --AMI=q431 --preExec "${preExecStringOne}" "${preExecStringTwo}" --conditionsTag="CONDBR2-BLKPA-RUN2-09" --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root | tee athenarunTwo.log
rc2=${PIPESTATUS[0]}
xAODDigest.py myAOD.pool.root | tee digestTwo.log
echo "art-result: $rc2 runTwo"

if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../runOne/ | tee diffEightThreads.log
 rcDiff=${PIPESTATUS[0]}
 collateDigest.sh digestTwo.log ../runOne/digestOne.log digestDiffOneTwo.log 
 echo "art-result: $rcDiff Diff-EightThreads-TwoRuns"
fi
