#!/bin/bash

# art-description: athenaHLT on EB data with Physics_pp_v7 menu, RAWtoESD on output, EDM checker on ESD, and Check of TrigDec 
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
 

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=HLT_physicsV7
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=RAWtoESD_physicsV7
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir  --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=TrigEDMCheck_physicsV7
export JOB_LOG="${NAME}.log"

timeout 50m trigtest_ART.pl --cleardir  --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=TrigDecTool_physicsV7
export JOB_LOG="${NAME}.log"

timeout 50m trigtest_ART.pl --cleardir  --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"


