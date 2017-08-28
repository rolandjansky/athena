#!/bin/bash

# art-description: Heavy ion MC v3 tight TriggerTest on MC
# art-type: grid

export NAME="mc_hi_v3_tight_grid"
export MENU="MC_HI_v3_tight_mc_prescale"
export EVENTS="500"
export INPUT="pbpb"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
