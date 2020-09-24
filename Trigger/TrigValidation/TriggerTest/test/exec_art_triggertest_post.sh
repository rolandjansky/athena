#!/bin/bash

echo  $(date "+%FT%H:%M %Z")"     Execute TriggerTest post processing for test ${NAME}"

### Find the branch name
if [ -n "${AtlasBuildBranch}" ]; then
  export BRANCH=${AtlasBuildBranch} # available after asetup
elif [ -n "${gitlabTargetBranch}" ]; then
  export BRANCH=${gitlabTargetBranch} # available in CI
else
  echo "WARNING Cannot determine the branch name, both variables AtlasBuildBranch and gitlabTargetBranch are empty"
fi

### DEFAULTS

if [ -z ${ATH_RETURN} ]; then
  echo "WARNING The env variable ATH_RETURN is not set, assuming 0"
  export ATH_RETURN=0
fi

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${TEST} ]; then
  export TEST="TriggerTest"
fi

if [ -z ${REF_FOLDER} ]; then
  export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/${TEST}/ref/${BRANCH}/test_${NAME}"
fi

### CHECKLOG

# if athena failed and we are running in CI, print the full log to stdout
if [ "${ATH_RETURN}" -ne "0" ] && [ -n "${gitlabTargetBranch}" ]; then
  echo "Printing the full ${JOB_LOG}"
  cat ${JOB_LOG}
fi

echo $(date "+%FT%H:%M %Z")"     Running checklog for errors"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG} 2>&1 | tee checklog.log
echo "art-result: ${PIPESTATUS[0]} CheckLog"

echo $(date "+%FT%H:%M %Z")"     Running checklog for warnings"
timeout 5m check_log.py --warnings --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG} >warnings.log 2>&1

### PERFMON

echo $(date "+%FT%H:%M %Z")"     Running perfmon"
timeout 5m perfmon.py -f 0.90 ntuple.pmon.gz

### HISTOGRAM COUNT

echo $(date "+%FT%H:%M %Z")"     Running histSizes"
timeout 5m histSizes.py -t expert-monitoring.root >histSizes.log 2>&1

### CHAINDUMP

echo $(date "+%FT%H:%M %Z")"     Running chainDump"
timeout 5m chainDump.py -f expert-monitoring.root --json >ChainDump.log 2>&1

### MAKE LOG TAIL FILE

export JOB_LOG_TAIL=${JOB_LOG%%.*}.tail.${JOB_LOG#*.}
tail -10000  ${JOB_LOG} > ${JOB_LOG_TAIL}

### ROOTCOMP

if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running rootcomp"
  timeout 10m rootcomp.py ${REF_FOLDER}/expert-monitoring.root expert-monitoring.root >rootcompout.log 2>&1
  echo "art-result: ${PIPESTATUS[0]} RootComp"
  echo $(date "+%FT%H:%M %Z")"     Running checkcounts"
  timeout 10m trigtest_checkcounts.sh 0 expert-monitoring.root ${REF_FOLDER}/expert-monitoring.root HLT 2>&1 | tee checkcountout.log
  echo "art-result: ${PIPESTATUS[0]} CheckCounts"
else
  echo $(date "+%FT%H:%M %Z")"     No reference expert-monitoring.root found in ${REF_FOLDER}"
  echo "art-result:  999 RootComp"
  echo "art-result:  999 CheckCounts"
fi

### ZEROCOUNTS CHECK

# SKIP_CHAIN_DUMP=1 skips this step
# SKIP_CHAIN_DUMP=2 skips the L1 and HLTTE checks, but does the HLTChain check
echo "trigedm SKIP_CHAIN_DUMP" $[SKIP_CHAIN_DUMP]
if [ $[SKIP_CHAIN_DUMP] != 1 ]; then 
   echo $(date "+%FT%H:%M %Z")"     Running check for zero L1, HLT or TE counts"
   export COUNT_EXIT=0
   if [ $[SKIP_CHAIN_DUMP] != 2 ]; then
      if [[ `sed 's|.*\(.* \)|\1|' L1AV.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then
         echo "L1 counts   ERROR  : all entires are ZERO please consult L1AV.txt"
         (( COUNT_EXIT = COUNT_EXIT || 1 ))
      fi
      if [[ `sed 's|.*\(.* \)|\1|' HLTTE.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then
         echo "HLTTE counts   ERROR  : all entires are ZERO please consult HLTTE.txt"
         (( COUNT_EXIT = COUNT_EXIT || 1 ))
      fi
   fi
   if [[ `sed 's|.*\(.* \)|\1|' HLTChain.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then
      echo "HLTChain counts   ERROR  : all entires are ZERO please consult HLTChain.txt"
      (( COUNT_EXIT = COUNT_EXIT || 1 ))
   fi
   echo "art-result: ${COUNT_EXIT} ZeroCounts"
else
  echo $(date "+%FT%H:%M %Z")"    Do not run ZERO counts for this test, SKIP_CHAIN_DUMP=1"
fi

### CHECKFILE

if [ -f ESD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on ESD"
  timeout 10m checkFile.py ESD.pool.root 2>&1 | tee ESD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileESD"
else 
  echo $(date "+%FT%H:%M %Z")"     No ESD.pool.root to check"
fi

if [ -f AOD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on AOD"
  timeout 10m checkFile.py AOD.pool.root 2>&1 | tee AOD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileAOD"
  echo $(date "+%FT%H:%M %Z")"     Running CheckxAOD AOD"
  timeout 10m checkxAOD.py AOD.pool.root 2>&1 | tee AOD.pool.root.checkxAODFile
  echo "art-result: ${PIPESTATUS[0]} CheckXAOD"
else 
  echo $(date "+%FT%H:%M %Z")"     No AOD.pool.root to check"
fi

### GENERATE JSON WITH POST-PROCESSING INFORMATION

echo $(date "+%FT%H:%M %Z")"     Running trig-test-json.py"
timeout 5m trig-test-json.py
cat extra-results.json && echo

### SUMMARY

echo  $(date "+%FT%H:%M %Z")"     Files in directory:"
ls -lh

echo  $(date "+%FT%H:%M %Z")"     Finished TriggerTest post processing for test ${NAME}"
