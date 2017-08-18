#!/bin/bash

# art-description: MinBias Enhanced (High Multiplicity Trigger) slice TriggerTest on MC
# art-type: grid

export SLICE="minbiasEnhanced"
export MENU="Physics_pp_v7"
export EVENTS="50"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
