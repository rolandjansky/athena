#!/bin/bash
# art-description: athenaMT trigger test on MC running L1 simulation and the LS2_v1 muon menu from TriggerMenuMT
# art-type: grid
# art-include: master/Athena
# art-output: *.log
# art-output: *.new
# art-output: *.txt
# art-output: *.root

export EVENTS=1000
export THREADS=1
export SLOTS=1
export INPUT="run2mc_ttbar"
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="doL1Sim=True;doEmptyMenu=True;doMuonSlice=True"


source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
