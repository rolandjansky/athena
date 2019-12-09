#!/bin/bash

# art-description: Physics v7 TriggerTest on MC to AOD
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
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
# art-output: costMonitoring_*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

export NAME="physics_pp_v7_primaries_aod_grid"
export MENU="Physics_pp_v7_primaries"
export EVENTS="500"
export JOBOPTION="TriggerTest/testCommonSliceAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
