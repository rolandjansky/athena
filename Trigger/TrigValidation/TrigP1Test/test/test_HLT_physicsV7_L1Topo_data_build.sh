#!/bin/bash

# art-description: athenaHLT on online test data including L1Topo ROI & DAQ ROB decoding with Physics_pp_v7 menu on events selected from data15_13TeV.00278748.physics_Main 
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

export JOB_LOG="HLT_physicsV7_L1Topo_data.log"

timeout 20m trigtest.pl --cleardir --test HLT_physicsV7_L1Topo_data --rundir HLT_physicsV7_L1Topo_data --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"



