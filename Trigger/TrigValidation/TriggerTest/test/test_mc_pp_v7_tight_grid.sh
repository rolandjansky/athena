#!/bin/bash

# art-description: MC v7 tight TriggerTest on MC
# art-type: grid

export NAME="mc_pp_v7_tight_grid"
export MENU="MC_pp_v7_tight_mc_prescale"
export EVENTS="500"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
