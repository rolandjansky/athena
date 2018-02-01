#!/bin/bash

# art-description:  Run athenaHLT on all events to create trigger configuration, generates and uploads SMKs created from the HLT first and compares after rerun jobs from DB 
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

export JOB_LOG="HLT_physicsV7_menu.log"

timeout 100m trigtest.pl --cleardir --test HLT_physicsV7_menu --rundir HLT_physicsV7_menu --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="UploadMenuKeys.log"

timeout 120m trigtest.pl --cleardir  --test UploadMenuKeys --rundir UploadMenuKeys --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="HLT_physicsV7_menu_rerundb.log"

timeout 100m trigtest.pl --cleardir  --test HLT_physicsV7_menu_rerundb --rundir HLT_physicsV7_menu_rerundb --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

