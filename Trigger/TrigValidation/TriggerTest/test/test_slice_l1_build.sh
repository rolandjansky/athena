#!/bin/bash

# art-description: No menu (L1) TriggerTest on MC
# art-type: build

export NAME="slice_l1_build"
export EVENTS="10"
export JOBOPTIONS="testLVL1CTPAthenaTrigRDO.py"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
