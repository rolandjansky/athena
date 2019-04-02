#!/bin/bash

# art-description: AthenaHLT on data, with MC_pp_V8 menu loose prescale set 
# art-type: build
# art-include: 21.3/Athena

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=HLT_mcV8
export JOB_LOG="${NAME}.log"

timeout 50m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"



