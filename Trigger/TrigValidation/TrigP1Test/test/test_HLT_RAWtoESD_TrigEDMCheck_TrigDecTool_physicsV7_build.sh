#!/bin/bash

# art-description: athenaHLT on EB data with Physics_pp_v7 menu, RAWtoESD on output, EDM checker on ESD, and Check of TrigDec 
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

export JOB_LOG="HLT_physicsV7.log"

timeout 100m trigtest.pl --cleardir --test HLT_physicsV7 --rundir HLT_physicsV7 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="RAWtoESD_physicsV7.log"

timeout 30m trigtest.pl --cleardir  --test RAWtoESD_physicsV7 --rundir RAWtoESD_physicsV7 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="TrigEDMCheck_physicsV7.log"

timeout 10m trigtest.pl --cleardir  --test TrigEDMCheck_physicsV7 --rundir TrigEDMCheck_physicsV7 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="TrigDecTool_physicsV7.log"

timeout 10m trigtest.pl --cleardir  --test TrigDecTool_physicsV7 --rundir TrigDecTool_physicsV7 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"


