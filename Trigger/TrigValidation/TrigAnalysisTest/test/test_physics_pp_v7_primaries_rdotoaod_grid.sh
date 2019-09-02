#!/bin/bash

# art-description: Trigger v7 primaries RDO to AOD test
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-cores: 8

unset ATHENA_NPROC_NUM

export NAME="physics_pp_v7_primaries_rdotoaod_grid"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="Physics_pp_v7_primaries"
export EVENTS="100"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
