#!/bin/bash

# art-description: Heavy ion physics v4 TriggerTest on MC
# art-type: grid

export NAME="physics_pp_v7_aod_grid"
export MENU="Physics_HI_v4"
export EVENTS="500"
export INPUT="pbpb"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
