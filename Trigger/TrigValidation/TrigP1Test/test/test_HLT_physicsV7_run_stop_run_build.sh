#!/bin/bash

# art-description: athenaHLT on data with physics_pp_V7 menu with stop/start transition 
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

  export JOB_LOG="HLT_physicsV7_run_stop_run.log"

timeout 100m trigtest.pl --cleardir --test HLT_physicsV7_run_stop_run --rundir HLT_physicsV7_run_stop_run --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"



