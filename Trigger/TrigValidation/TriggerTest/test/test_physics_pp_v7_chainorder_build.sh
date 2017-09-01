#!/bin/bash

# art-description: TriggerTest on changes coming through the order of execution
# art-type: build

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

return trigtest_checkcounts.sh expert-monitoring-ascend.root 0 BOTH
