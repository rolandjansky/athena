#!/bin/bash

# art-description: stand-alone test of the steering with the PrescaleMenu 
# art-type: build
# art-include: 21.3/Athena
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1
# art-include: master/Athena

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=pureSteering_prescaling
export JOB_LOG="${NAME}.log"

timeout 50m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"



