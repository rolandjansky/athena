#!/bin/bash

# art-description: MC v8 TriggerTest on data
# art-type: grid
# art-include: 21.3/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: costMonitoring_*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

export NAME="data_mc_pp_v8_grid"
export MENU="MC_pp_v8"
export EVENTS="1000"
export INPUT="data"

# The flag below skips HLTTE and L1 counts checks, but keeps the HLTChain counts check.
# This is needed because we run on data without rerunLvl1, so we only produce HLTChain counts.
export SKIP_CHAIN_DUMP=2

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
