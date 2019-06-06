#!/bin/bash
# art-description: athenaMT trigger test using the full LS2_v1 menu from TriggerMenuMT
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.new
# art-output: *.txt
# art-output: *.root

export EVENTS=20
export THREADS=1
export SLOTS=1
export INPUT="ttbar"
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="doL1Sim=True"

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
