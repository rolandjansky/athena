#!/bin/bash

# art-description: Physics v7 tight TriggerTest on MC
# art-type: grid

export MENU="Physics_pp_v7_tight_physics_prescale"
export EVENTS="500"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
