#!/bin/bash

# art-description: HLTMPPU on data with Physics_pp_v7 menu currently cosmics from 2016 
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

export NAME=HLT_physicsV7_COS_2016IDCOSMICS
export JOB_LOG="${NAME}.log"

timeout 30m trigtest_ART_HLTMPPU.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART_HLTMPPU.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"



