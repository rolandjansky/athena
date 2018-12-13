#!/bin/bash

# art-description:  Run athenaHLT on all events to create trigger configuration, generates and uploads SMKs created from the HLT first and compares after rerun jobs from DB, then compare the SMK created from the HLT_physicsV7 first and rerun jobs. Runs the same thing also for the physics HI v5 menu
# art-type: build
# art-include: 21.3/Athena

if [ -z ${TEST} ]; then
  export TEST="TrigP1Test"
fi

export NAME=HLT_physicsV7_menu
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=UploadMenuKeys
export JOB_LOG="${NAME}.log"

timeout 120m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV7_menu_rerundb
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV7_menu_checkkeys
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=CheckKeysV7
export JOB_LOG="${NAME}.log"

timeout 180m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV7_rerunLVL1_menu
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=UploadMenuKeys_rerunLVL1
export JOB_LOG="${NAME}.log"

timeout 120m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

export NAME=HLT_physicsV7_rerunLVL1_menu_rerundb
export JOB_LOG="${NAME}.log"

timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

#export NAME=HLT_HIV5_menu
#export JOB_LOG="${NAME}.log"

#timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

#ATH_RETURN=${PIPESTATUS[0]}
#echo "art-result: ${ATH_RETURN} ${NAME}"

#export NAME=UploadMenuKeys_HI
#export JOB_LOG="${NAME}.log"

#timeout 120m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

#ATH_RETURN=${PIPESTATUS[0]}
#echo "art-result: ${ATH_RETURN} ${NAME}"

#export NAME=HLT_HIV5_menu_rerundb
#export JOB_LOG="${NAME}.log"

#timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

#ATH_RETURN=${PIPESTATUS[0]}
#echo "art-result: ${ATH_RETURN} ${NAME}"

#export NAME=HLT_HIV5_menu_checkkeys
#export JOB_LOG="${NAME}.log"

#timeout 100m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

#ATH_RETURN=${PIPESTATUS[0]}
#echo "art-result: ${ATH_RETURN} ${NAME}"

#export NAME=CheckKeysHIV5
#export JOB_LOG="${NAME}.log"

#timeout 180m trigtest_ART.pl --cleardir --test ${NAME} --rundir ${NAME} --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

#ATH_RETURN=${PIPESTATUS[0]}
#echo "art-result: ${ATH_RETURN} ${NAME}"


