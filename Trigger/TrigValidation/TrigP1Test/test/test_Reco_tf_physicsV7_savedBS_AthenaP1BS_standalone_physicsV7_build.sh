#!/bin/bash

# art-description: athenaHLT on EB data with Physics_pp_v7 menu, Reco_tf on saved output, and Standalone HLT running in athena and compare to athenaHLT 
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

export JOB_LOG="HLT_physicsV7.log"

timeout 100m trigtest.pl --cleardir --test HLT_physicsV7 --rundir HLT_physicsV7 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="Reco_tf_physicsV7_savedBS.log"

timeout 30m trigtest.pl --cleardir  --test Reco_tf_physicsV7_savedBS --rundir Reco_tf_physicsV7_savedBS --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="AthenaP1BS_standalone.log"

timeout 45m trigtest.pl --cleardir  --test AthenaP1BS_standalone --rundir AthenaP1BS_standalone --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"



