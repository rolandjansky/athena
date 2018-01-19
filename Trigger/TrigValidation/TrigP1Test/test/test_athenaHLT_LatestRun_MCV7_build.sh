#!/bin/bash

# art-description: athenaHLT on latest available run
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1

export NAME="athenaHLT_LatestRun_MCV7_build"
export ATHENAHLTCMD="trigp1test_athenaHLT.py"
export ATHENAHLTOPT="--nevents=5 --nfiles=1 --dir='/eos/atlas/atlastier0/rucio/data17_13TeV/physics_Main/' --modifiers='testMCV7=True;fpeAuditor=True;from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties ; pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True'"

timeout 40m source exec_athenaHLT_art_trigger_validation.sh
timeout 10m source exec_art_trigp1test_post.sh

