#!/bin/bash

echo  $(date "+%FT%H:%M %Z")"     Execute TrigP1Test post processing"

### DEFAULTS
if [ -z ${NAME} ]; then
  export NAME="TrigP1Test"
fi

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

if [ -n "${AtlasBuildBranch}" ]; then
  export BRANCH=${AtlasBuildBranch} # available after asetup
elif [ -n "${gitlabTargetBranch}" ]; then
  export BRANCH=${gitlabTargetBranch} # available in CI
else
  echo "WARNING Cannot determine the branch name, both variables AtlasBuildBranch and gitlabTargetBranch are empty"
fi

if [ -z ${REF_FOLDER} ]; then
  export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/${TEST}/ref/${BRANCH}/${NAME}"
fi

export ROOTCOMP_CMD="rootcomp.py --pdf --noRoot"
export ROOTCOMP_LOG="rootcompout.log"

###
echo $(date "+%FT%H:%M %Z")"     Checking for athenaHLT log"
if [ -f ${JOB_athenaHLT_LOG} ]; then
   echo "art-result: 0 ${NAME}.MissingLog"
else
   echo "Cannot find log file"
   echo "art-result: 1 ${NAME}.MissingLog"
fi

# mother process exit code
grep "art-result:" ${JOB_athenaHLT_LOG}

echo $(date "+%FT%H:%M %Z")"     Checking for timeouts in child processes"
if [ -f ${JOB_athenaHLT_LOG} ]; then
   checkTimeut=`grep "CRITICAL stopped by user interrupt|ERROR Keyboard interruption caught|Signal handler: Killing [0-9]+ with 15" ${JOB_athenaHLT_LOG}`
   if [[ -z "${checkTimeut}" ]]; then
     echo "art-result: 0 ${NAME}.ChildTimeout"
   else
     echo "Timeout found: "
     echo ${checkTimeut}
     echo "art-result: 1 ${NAME}.ChildTimeout"
   fi
fi 

echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 1m check_log.pl --config checklogTrigP1Test.conf --showexcludestats ${JOB_athenaHLT_LOG} 2>&1 | tee -a checklog.log
echo "art-result: ${PIPESTATUS[0]} ${NAME}.CheckLog"

# TODO
# add check_statuscode.py ${JOB_LOG}
# RunMsgFinder

# Run perfmon
if [ -f ntuple.pmon.gz ]; then
  timeout 1m perfmon.py -f 0.90 ntuple.pmon.gz
  timeout 1m convert -density 300 -trim ntuple.perfmon.pdf -quality 100 -resize 50% ntuple.perfmon.png
fi 

if [ -f expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running chainDump"
  timeout 1m chainDump.py -S --rootFile=expert-monitoring.root

  echo $(date "+%FT%H:%M %Z")"     Running check for zero L1, HLT or TE counts"
  export COUNT_EXIT=0
  if [[ `sed 's|.*\(.* \)|\1|' L1AV.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then
    echo "L1 counts   ERROR  : all entires are ZERO please consult L1AV.txt"
    (( COUNT_EXIT = COUNT_EXIT || 1 ))
  fi
  if [[ `sed 's|.*\(.* \)|\1|' HLTChain.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then
    echo "HLTChain counts   ERROR  : all entires are ZERO please consult HLTChain.txt"
    (( COUNT_EXIT = COUNT_EXIT || 1 ))
  fi
  if [[ `sed 's|.*\(.* \)|\1|' HLTTE.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then
    echo "HLTTE counts   ERROR  : all entires are ZERO please consult HLTTE.txt"
    (( COUNT_EXIT = COUNT_EXIT || 1 ))
  fi
  echo "art-result: ${COUNT_EXIT} ${NAME}.ZeroCounts"

  if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
    echo $(date "+%FT%H:%M %Z")"     Running rootcomp"
    timeout 10m ${ROOTCOMP_CMD} ${REF_FOLDER}/expert-monitoring.root 2>&1 | tee -a ${ROOTCOMP_LOG}
    echo "art-result: ${PIPESTATUS[0]} ${NAME}.RootComp"
    echo $(date "+%FT%H:%M %Z")"     Running checkcounts"
    timeout 10m trigtest_checkcounts.sh 0 expert-monitoring.root ${REF_FOLDER}/expert-monitoring.root HLT 2>&1 | tee -a checkcountout.log
    echo "art-result: ${PIPESTATUS[0]} ${NAME}.CheckCounts"
  else
    echo $(date "+%FT%H:%M %Z")"     No reference expert-monitoring.root found in ${REF_FOLDER}"
  fi

else
  echo $(date "+%FT%H:%M %Z")"     No expert-monitoring.root file found. Skipping chainDump.py, RootComp and CheckCounts"
fi

export JOB_LOG_TAIL=${JOB_LOG%%.*}.tail.${JOB_LOG#*.}
tail -10000  ${JOB_LOG} > ${JOB_LOG_TAIL}

grep REGTEST ${JOB_LOG} > athena.regtest
if [ -f ${REF_FOLDER}/athena.regtest ]; then
  echo $(date "+%FT%H:%M %Z")"     Running regtest"
  timeout 1m regtest.pl --inputfile athena.regtest --reffile ${REF_FOLDER}/athena.regtest 2>&1 | tee -a regtest.log
  echo "art-result: ${PIPESTATUS[0]} ${NAME}.RegTest"
else
  echo $(date "+%FT%H:%M %Z")"     No reference athena.regtest found in ${REF_FOLDER}"
fi

if [ -f trig_cost.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CostMon"
#  timeout 2h RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode --ratesMode > costMon.log 2>&1
  timeout 2h RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode --monitorRates --isCPUPrediction --useEBWeight --patternsMonitor HLT_costmonitor HLT_mistimemonj400 HLT_mistimemoncaltimenomu HLT_mistimemoncaltime HLT_l1topodebug HLT_l1calooverflow HLT_e5_lhvloose_nod0_bBeexM6000t HLT_2e5_lhvloose_nod0_bBeexM6000t HLT_cscmon_L1All HLT_j0_perf_ds1_L1J100 --patternsInvert  --predictionLumi 1.50e34 > costMon.log 2>&1
else 
  echo $(date "+%FT%H:%M %Z")"     file trig_cost.root does not exist thus RunTrigCostD3PD will not be run"
fi

if [ -f ESD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on ESD"
  timeout 10m checkFile.py ESD.pool.root 2>&1 | tee -a ESD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} ${NAME}.CheckFileESD"
else 
  echo $(date "+%FT%H:%M %Z")"     No ESD.pool.root to check"
fi

if [ -f AOD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on AOD"
  timeout 10m checkFile.py AOD.pool.root 2>&1 | tee -a AOD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} ${NAME}.CheckFileAOD"
  echo $(date "+%FT%H:%M %Z")"     Running CheckxAOD AOD"
  timeout 10m checkxAOD.py AOD.pool.root 2>&1 | tee -a AOD.pool.root.checkxAODFile
  echo "art-result: ${PIPESTATUS[0]} ${NAME}.CheckFileXAOD"
else 
  echo $(date "+%FT%H:%M %Z")"     No AOD.pool.root to check"
fi

echo  $(date "+%FT%H:%M %Z")"     Files in directory:"
ls -lh

echo  $(date "+%FT%H:%M %Z")"     Finished TrigP1Test post processing"

