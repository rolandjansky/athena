#!/bin/bash

# art-description:  Run athenaHLT on all events to create trigger configuration, generates and uploads SMKs created from the HLT first and compares after rerun jobs from DB, then compare the SMK created from the HLT_physicsV7 first and rerun jobs.Only in 21.1! 
# art-type: build
# art-include: 21.1/AthenaP1

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=HLT_physicsV7_menu
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=UploadMenuKeys_ART
export JOB_LOG="${NAME}.log"

timeout 120m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

