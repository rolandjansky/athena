#!/bin/bash
# art-description: athenaMT trigger test with bjet ALLTE chains
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.new
# art-output: *.txt
# art-output: *.root

export SKIPEVENTS=0
export EVENTS=100
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/bjet_menu_ALLTE.py"
export REGTESTEXP="TrigSignatureMoniMT.*HLT_.*"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/bjet_menu_ALLTE.ref`

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
