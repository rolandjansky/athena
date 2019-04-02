#!/bin/bash

# art-description:  Run athenaHLT on all events to create trigger configuration, generates and uploads SMKs with rerunLVL1 physics menu, then run ROS simulation with prescale keys for 17000 and 9000. RB configured such that new chains are unprescaled. Only in 21.1! 
# art-type: build
# art-include: 21.3/Athena

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=HLT_physicsV8_menu
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=UploadMenuKeys_ART
export JOB_LOG="${NAME}.log"

timeout 120m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV8_ROSsim_17000
export JOB_LOG="${NAME}.log"

timeout 150m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV8_ROSsim_9000
export JOB_LOG="${NAME}.log"

timeout 150m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

