#!/bin/bash
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Trigger ART test script with post-exec checks based on TriggerTest/test/exec_art_triggertest_post.sh
#

echo  $(date "+%FT%H:%M %Z")"     Execute TrigUpgradeTest post processing for test ${NAME}"

### DEFAULTS

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${TEST} ]; then
  export TEST="TrigUpgradeTest"
fi

if [ -z ${REF_FOLDER} ]; then
  # Try eos first
  export REF_FOLDER="/eos/atlas/atlascerngroupdisk/data-art/grid-input/${TEST}/ref/${AtlasBuildBranch}/test_${NAME}"
  # If not available, try cvmfs
  if [ -d ${REF_FOLDER} ]; then
    echo "Using reference directory from eos: ${REF_FOLDER}"
  else
    export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/${TEST}/ref/${AtlasBuildBranch}/test_${NAME}"
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

###

echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 1m check_log.pl --config checklogTrigUpgradeTest.conf --showexcludestats ${JOB_LOG} | tee checklog.log

echo "art-result: ${PIPESTATUS[0]} CheckLog"

###

export JOB_LOG_TAIL=${JOB_LOG%%.*}.tail.${JOB_LOG#*.}
tail -10000  ${JOB_LOG} > ${JOB_LOG_TAIL}

###

grep -E "${REGTESTEXP}" ${JOB_LOG} > athena.regtest

if [ -f ${REF_FOLDER}/athena.regtest ]; then
  echo $(date "+%FT%H:%M %Z")"     Running regtest"
  timeout 1m regtest.pl --inputfile athena.regtest --reffile ${REF_FOLDER}/athena.regtest --linematch ".*" | tee regtest.log
  echo "art-result: ${PIPESTATUS[0]} RegTest"
else
  echo $(date "+%FT%H:%M %Z")"     No reference athena.regtest found in ${REF_FOLDER}"
  echo "art-result: 999 RegTest"
fi

mv athena.regtest athena.regtest.new

if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running rootcomp"
  timeout 10m rootcomp.py --skip="TIME_" ${REF_FOLDER}/expert-monitoring.root expert-monitoring.root | tee rootcompout.log
  echo "art-result: ${PIPESTATUS[0]} RootComp"
else
  echo $(date "+%FT%H:%M %Z")"     No reference expert-monitoring.root found in ${REF_FOLDER}"
  echo "art-result:  999 RootComp"
fi

###

echo  $(date "+%FT%H:%M %Z")"     Files in directory:"
ls -lh

echo  $(date "+%FT%H:%M %Z")"     Finished TrigUpgradeTest post processing for test ${NAME}"

