#!/bin/bash

# art-description: Physics v7 TriggerTest on MC to AOD
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

export NAME="physics_pp_v7_aod_grid"
export MENU="Physics_pp_v7"
export EVENTS="500"
export JOBOPTION="testCommonSliceAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
