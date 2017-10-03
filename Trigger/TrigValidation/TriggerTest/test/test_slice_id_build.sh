#!/bin/bash

# art-description: ID TriggerTest over e28_lhtight_idperf on MC
# art-type: build

export NAME="slice_id_build"
export EVENTS="10"
export JOBOPTIONS="testIDAthenaTrigRDO.py"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
