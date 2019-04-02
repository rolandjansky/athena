#!/bin/bash

# art-description: athenaHLT on th physics menu without FTK chains on data with and without FTK sim
# art-type: build
# art-include: 21.3/Athena

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=HLT_physicsV8
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV8_FTKdata
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

