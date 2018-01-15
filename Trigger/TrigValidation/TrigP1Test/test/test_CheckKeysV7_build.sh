#!/bin/bash

# art-description: compare the SMK created from the HLT_physicsV7 first and rerun jobs 
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

export JOB_LOG="HLT_physicsV7_menu.log"

timeout 100m trigtest.pl --cleardir --test HLT_physicsV7_menu --rundir HLT_physicsV7_menu --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="HLT_physicsV7_rerun.log"

timeout 100m trigtest.pl --cleardir  --test HLT_physicsV7_rerun --rundir HLT_physicsV7_rerun --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="CheckKeysV7.log"

timeout 180m trigtest.pl --cleardir  --test CheckKeysV7 --rundir CheckKeysV7 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

