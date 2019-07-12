#!/bin/bash
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Trigger ART test script with post-exec checks based on TriggerTest/test/exec_art_triggertest_post.sh
#

echo  $(date "+%FT%H:%M %Z")"     Execute TrigUpgradeTest post processing for test ${NAME}"

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
  export TEST="TrigUpgradeTest"
fi

if [ -z ${ESDTOCHECK} ]; then
  export ESDTOCHECK="ESD.pool.root"
fi

if [ -z ${AODTOCHECK} ]; then
  export AODTOCHECK="AOD.pool.root"
fi

if [ -z ${REF_FOLDER} ]; then
  # Try eos first
  export REF_FOLDER="/eos/atlas/atlascerngroupdisk/data-art/grid-input/${TEST}/ref/${BRANCH}/test_${NAME}"
  # If not available, try cvmfs
  if [ -d ${REF_FOLDER} ]; then
    echo "Using reference directory from eos: ${REF_FOLDER}"
  else
    export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/${TEST}/ref/${BRANCH}/test_${NAME}"
    if [ -d ${REF_FOLDER} ]; then
      echo "Reference from eos unavailable, using one from cvmfs: ${REF_FOLDER}"
    else
      echo "Reference directory for test ${NAME} not available from eos or cvmfs"
    fi
  fi
fi

# Note REGTESTEXP is a GNU grep regexp, not perl regexp
if [ -z "${REGTESTEXP}" ]; then
  export REGTESTEXP="REGTEST"
fi

# Option to override the regtest reference location
if [ -z "${REGTESTREF}" ]; then
  export REGTESTREF=${REF_FOLDER}/athena.regtest
fi

### CHECKLOG

# if athena failed and we are running in CI, print the full log to stdout
if [ "${ATH_RETURN}" -ne "0" ] && [ -n "${gitlabTargetBranch}" ]; then
  echo "Printing the full ${JOB_LOG}"
  cat ${JOB_LOG}
fi

echo $(date "+%FT%H:%M %Z")"     Running checklog for errors"
timeout 5m check_log.pl --config checklogTrigUpgradeTest.conf --showexcludestats ${JOB_LOG} 2>&1 | tee checklog.log
echo "art-result: ${PIPESTATUS[0]} CheckLog"

echo $(date "+%FT%H:%M %Z")"     Running checklog for warnings"
timeout 5m check_log.pl --config checklogTrigUpgradeTest.conf --noerrors --warnings --showexcludestats ${JOB_LOG} >warnings.log 2>&1

### PERFMON

if [ -f ntuple.pmon.gz ]; then
  echo $(date "+%FT%H:%M %Z")"     Running perfmon"
  timeout 5m perfmon.py -f 0.90 ntuple.pmon.gz
fi

### HISTOGRAM COUNT

if [ -f expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running histSizes"
  timeout 5m histSizes.py -t expert-monitoring.root >histSizes.log 2>&1
fi

### MAKE LOG TAIL FILE

export JOB_LOG_TAIL=${JOB_LOG%%.*}.tail.${JOB_LOG#*.}
tail -10000  ${JOB_LOG} > ${JOB_LOG_TAIL}

### REGTEST

REGTESTREF_BASENAME=$(basename -- "${REGTESTREF}")
if [ -z "${REGTESTEXP_EXCLUDE}" ]; then
  grep -E "${REGTESTEXP}" ${JOB_LOG} > "${REGTESTREF_BASENAME}"
else
  grep -E "${REGTESTEXP}" ${JOB_LOG} | grep -v -E "${REGTESTEXP_EXCLUDE}" > "${REGTESTREF_BASENAME}"
fi

if [ -f ${REGTESTREF} ]; then
  echo $(date "+%FT%H:%M %Z")"     Running regtest using reference file ${REGTESTREF}"
  timeout 5m regtest.pl --inputfile ${REGTESTREF_BASENAME} --reffile ${REGTESTREF} --linematch ".*" 2>&1 | tee regtest.log
  echo "art-result: ${PIPESTATUS[0]} RegTest"
else
  echo $(date "+%FT%H:%M %Z")"     The reference file does not exist: ${REGTESTREF}"
  echo "art-result: 999 RegTest"
fi

mv ${REGTESTREF_BASENAME} ${REGTESTREF_BASENAME}.new

### ROOTCOMP

if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running rootcomp"
  timeout 10m rootcomp.py --skip="TIME_" ${REF_FOLDER}/expert-monitoring.root expert-monitoring.root >rootcompout.log 2>&1
  echo "art-result: ${PIPESTATUS[0]} RootComp"
else
  echo $(date "+%FT%H:%M %Z")"     No reference expert-monitoring.root found in ${REF_FOLDER}"
  echo "art-result: 999 RootComp"
fi

### CHAINDUMP
# SKIP_CHAIN_DUMP=1 skips this step

# Using temporary workaround to dump HLTChain.txt
if [ -f expert-monitoring.root ] && [ $[SKIP_CHAIN_DUMP] != 1 ]; then
  echo "Running chainDumpWorkaround.sh"
  chainDumpWorkaround.sh expert-monitoring.root
fi

### CHECKFILE

if [ -f ${ESDTOCHECK} ]; then
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on ESD"
  timeout 10m checkFile.py ${ESDTOCHECK} 2>&1 | tee ${ESDTOCHECK}.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileESD"
else
  echo $(date "+%FT%H:%M %Z")"     No ESD file to check"
fi

if [ -f ${AODTOCHECK} ]; then
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on AOD"
  timeout 10m checkFile.py ${AODTOCHECK} 2>&1 | tee ${AODTOCHECK}.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileAOD"
  echo $(date "+%FT%H:%M %Z")"     Running CheckxAOD AOD"
  timeout 10m checkxAOD.py ${AODTOCHECK} 2>&1 | tee ${AODTOCHECK}.checkxAOD
  echo "art-result: ${PIPESTATUS[0]} CheckXAOD"
else
  echo $(date "+%FT%H:%M %Z")"     No AOD file to check"
fi

### GENERATE JSON WITH POST-PROCESSING INFORMATION

echo $(date "+%FT%H:%M %Z")"     Running trig-test-json.py"
timeout 5m trig-test-json.py
cat extra-results.json && echo

### SUMMARY

echo  $(date "+%FT%H:%M %Z")"     Files in directory:"
ls -lh

echo  $(date "+%FT%H:%M %Z")"     Finished TrigUpgradeTest post processing for test ${NAME}"
