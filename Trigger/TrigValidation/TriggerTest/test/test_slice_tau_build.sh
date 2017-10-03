#!/bin/bash

# art-description: Tau slice TriggerTest on MC
# art-type: build

export NAME="slice_tau_build"
export SLICE="tau"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
