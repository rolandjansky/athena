#!/bin/bash

# art-description: Physics v7 TriggerTest on MC
# art-type: grid

export NAME="physics_pp_v7_grid"
export MENU="Physics_pp_v7"
export EVENTS="500"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
