#!/bin/bash

# art-description: MinBias Enhanced (High Multiplicity Trigger) slice TriggerTest on MC
# art-type: grid
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

export NAME="slice_minbias_hmt_grid"
export SLICE="minbiasEnhanced"
export MENU="Physics_pp_v7"
export EVENTS="50"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
