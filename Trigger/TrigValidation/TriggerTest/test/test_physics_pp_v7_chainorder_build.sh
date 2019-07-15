#!/bin/bash

# art-description: TriggerTest on changes coming through the order of execution
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

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
export JOB_LOG_2=${JOB_LOG%%.*}2.${JOB_LOG#*.}
export JOB_LOG=${JOB_LOG_2}
source exec_athena_art_trigger_validation.sh

timeout 10m rootcomp.py expert-monitoring-ascend.root expert-monitoring.root | tee rootcompout.log
echo "art-result: ${PIPESTATUS[0]} RootComp" 

trigtest_checkcounts.sh 0 expert-monitoring-ascend.root HLT | tee checkcountout.log
echo "art-result: ${PIPESTATUS[0]} CheckCounts"
