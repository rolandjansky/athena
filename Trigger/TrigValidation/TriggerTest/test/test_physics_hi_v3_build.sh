#!/bin/bash

# art-description: Heavy ion physics v3 TriggerTest on MC
# art-type: build

export MENU="Physics_HI_v3"
export EVENTS="5"
export INPUT="pbpb"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
