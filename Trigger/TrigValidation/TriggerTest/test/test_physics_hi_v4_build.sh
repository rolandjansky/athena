#!/bin/bash

# art-description: Heavy ion physics v4 TriggerTest on MC
# art-type: build

export NAME="physics_hi_v4_build"
export MENU="Physics_HI_v4"
export EVENTS="5"
export INPUT="pbpb"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
