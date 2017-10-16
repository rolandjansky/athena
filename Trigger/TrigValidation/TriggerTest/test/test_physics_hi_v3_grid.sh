#!/bin/bash

# art-description: Heavy ion physics v3 TriggerTest on MC
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

export NAME="physics_hi_v3_grid"
export MENU="Physics_HI_v3"
export EVENTS="500"
export INPUT="pbpb"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
