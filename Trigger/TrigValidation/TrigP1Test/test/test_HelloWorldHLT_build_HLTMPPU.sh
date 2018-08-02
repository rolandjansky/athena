#!/bin/bash

# art-description: HelloWorld in HLTMPPU
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1
# art-include: master/Athena

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

source HLTMPPU_setup.sh

export NAME=HelloWorldHLT
export JOB_LOG="${NAME}.log"

timeout 5m trigtest_ART_HLTMPPU.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART_HLTMPPU.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

