#!/bin/bash

# art-description: MC v7 TriggerTest on MC
# art-type: build

export MENU="MC_pp_v7"
export EVENTS="5"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
