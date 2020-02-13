#!/bin/bash
# art-description: athenaMT trigger test on MC running L1 simulation and the LS2_v1 muon menu from TriggerMenuMT
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*

export EVENTS=1000
export THREADS=1
export SLOTS=1
export INPUT="ttbar"
export JOBOPTION="TriggerJobOpts/runHLT_standalone.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="doL1Sim=True;doEmptyMenu=True;doMuonSlice=True;doWriteBS=False;doWriteRDOTrigger=True;"


source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
