#!/bin/bash

# art-description: Physics v8 TriggerTest on data
# art-type: grid
# art-include: 21.3/Athena
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig_*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: costMonitoring_*
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: TotalEventsProcessed.txt
# art-output: *.regtest.new

export NAME="data_physics_pp_v8_grid"
export MENU="Physics_pp_v8"
export EVENTS="500"
export INPUT="data"

# The flag below skips HLTTE and L1 counts checks, but keeps the HLTChain counts check.
# This is needed because we run on data without rerunLvl1, so we only produce HLTChain counts.
export SKIP_CHAIN_DUMP=2

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
