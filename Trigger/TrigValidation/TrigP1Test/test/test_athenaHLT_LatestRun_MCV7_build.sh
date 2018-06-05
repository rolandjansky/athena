#!/bin/bash

# art-description: athenaHLT on latest available run
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1
# art-include: master/Athena

export NAME="athenaHLT_LatestRun_MCV7_build"

timeout 40m trigp1test_athenaHLT.py --nfiles=1 --dir='/eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/' --modifiers="testMCV7=True;fpeAuditor=True;from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties ; pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True"

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

timeout 10m exec_art_trigp1test_post.sh

