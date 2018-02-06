#!/bin/bash

# art-description: Trig_reco_tf using debug_stream setup as is run at T0 
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=Trigreco_dbgstream_T0
export JOB_LOG="${NAME}.log"

timeout 30m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"



