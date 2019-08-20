#!/bin/bash

# art-description: MC v8 TriggerTest on MC
# art-type: build
# art-include: 21.3/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_build"
export MENU="MC_pp_v8"
export EVENTS="5"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
