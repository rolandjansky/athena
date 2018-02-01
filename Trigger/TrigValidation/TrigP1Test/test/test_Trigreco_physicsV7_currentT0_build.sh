#!/bin/bash

# art-description: Trig_reco_tf with current T0 config, EDMCheck on ESD and AOD  
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

export JOB_LOG="Trigreco_physicsV7_currentT0.log"

timeout 45m trigtest.pl --cleardir --test Trigreco_physicsV7_currentT0 --rundir Trigreco_physicsV7_currentT0 --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="TrigEDMCheck_physicsV7_currentT0_ESD.log"

timeout 10m trigtest.pl --cleardir  --test TrigEDMCheck_physicsV7_currentT0_ESD --rundir TrigEDMCheck_physicsV7_currentT0_ESD --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

export JOB_LOG="TrigEDMCheck_physicsV7_currentT0_AOD.log"

timeout 10m trigtest.pl --cleardir  --test TrigEDMCheck_physicsV7_currentT0_AOD --rundir TrigEDMCheck_physicsV7_currentT0_AOD --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"

