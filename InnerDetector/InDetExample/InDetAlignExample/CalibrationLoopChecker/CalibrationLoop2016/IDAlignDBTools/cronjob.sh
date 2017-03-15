#!/bin/bash

project=data16_13TeV
Tier0LogFile=/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/${project}/GoodEndedRuns/GoodEndedRuns.txt
CheckerLogFile=/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log/CalibLoopChecker_log.txt
HandShakeLogFile=/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log/HandShake_log.txt
CronJobExecLogFile=/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log/CronjobExec_log.txt

cd /afs/cern.ch/user/a/atlidali/CalibrationLoop2016/IDAlignDBTools

echo "----------------------------------------------------------------------------------------------------" >> ${CronJobExecLogFile}
echo `date` - `date +%s` - executed a cronjob.  >> ${CronJobExecLogFile}

#sh check_missing_runs.sh

./compare_logs.py ${Tier0LogFile} ${HandShakeLogFile} | sort | while read runNumber
do
    echo `date` - `date +%s` "- ${runNumber} is a candidate run to process."	>> ${CronJobExecLogFile}
    echo `date` - `date +%s` "-   ==> Execute run_checker.sh for ${runNumber}."	>> ${CronJobExecLogFile}
    ./run_checker.sh ${runNumber} CRONJOB --exec &>lastlog.txt
    #./run_checker.sh ${runNumber} CRONJOB &>lastlog.txt
    echo `date` - `date +%s` "-   ==> run_checker.sh ${runNumber} is done."	>> ${CronJobExecLogFile}
done

echo `date` - `date +%s` - cronjob is done.  >> ${CronJobExecLogFile}
