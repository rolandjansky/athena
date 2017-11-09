#!/bin/bash

# art-description: TriggerTest on changes coming through the order of execution
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
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

export NAME="physics_pp_v7_chainorder_build"
export MENU="Physics_pp_v7"
export EVENTS="10"
export EXTRA="chainOrderOption=-1;"
export COST_MONITORING="False"

# Run with ascending
source exec_athena_art_trigger_validation.sh
mv expert-monitoring.root expert-monitoring-ascend.root

export EXTRA="chainOrderOption=1;"

# Run with decending
source exec_athena_art_trigger_validation.sh

rootcomp.py expert-monitoring-ascend.root expert-monitoring.root

trigtest_checkcounts.sh expert-monitoring-ascend.root 0 BOTH
