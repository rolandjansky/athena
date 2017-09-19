#!/bin/bash

# art-description: No menu (L1) TriggerTest on MC
# art-type: grid

export NAME="slice_l1_grid"
export JOBOPTIONS="testLVL1CTPAthenaTrigRDO.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
