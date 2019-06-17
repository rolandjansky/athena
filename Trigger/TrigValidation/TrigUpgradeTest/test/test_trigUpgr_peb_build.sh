#!/bin/bash
# art-description: athenaMT trigger test of the data scouting (TLA) infrastructure
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.new
# art-output: *.txt
# art-output: *.root

export SKIPEVENTS=10
export EVENTS=20
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/pebTest.py"
export REGTESTEXP="obeysLB=.*robs=.*dets = |adds PEBInfo|TriggerSummary.*HLT_.*"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/peb.ref`

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
