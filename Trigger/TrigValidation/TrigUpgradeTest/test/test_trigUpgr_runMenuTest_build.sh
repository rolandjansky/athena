#!/bin/bash
# art-description: athenaMT trigger test using runMenuTest.py job options
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
export JOBOPTION="TrigUpgradeTest/runMenuTest.py"
export REGTESTEXP=".*ERROR (?\!attempt to add a duplicate).*|(?\!IOVSvcTool).*FATAL.*|TrigSignatureMoniMT .*INFO.*"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/runMenuTest.ref`

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
