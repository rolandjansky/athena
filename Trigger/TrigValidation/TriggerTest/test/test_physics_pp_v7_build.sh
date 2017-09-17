#!/bin/bash

# art-description: Physics v7 TriggerTest on MC
# art-type: build

export NAME="physics_pp_v7_build"
export MENU="Physics_pp_v7"
export EVENTS="5"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
