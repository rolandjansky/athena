#!/bin/bash

# art-description: testing trigger results w/wo merging L2/EF, based on AthenaTrigRDO with run_standalone.py 
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

export JOB_LOG="AthenaTrigBS_L2EFMerging.log"

timeout 100m trigtest.pl --cleardir --test AthenaTrigBS_L2EFMerging --rundir AthenaTrigBS_L2EFMerging --conf TrigP1Test_ART.conf | tee ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"



