#!/bin/bash

# art-description: MC v8 TriggerTest on FTK MC to AOD
# art-type: build
# art-include: 21.3/Athena
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: costMonitoring_*
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: TotalEventsProcessed.txt
# art-output: *.regtest.new

export NAME="mc_pp_v8_ftk_build"
export MENU="MC_pp_v8"
export INPUT="ftk"
export JOBOPTIONS="TriggerTest/testCommonSliceAthenaTrigRDOtoAOD.py"
export EVENTS="5"
export COST_MONITORING="False"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.doFTK=True;from RecExConfig.RecFlags import rec;rec.doFTK.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
