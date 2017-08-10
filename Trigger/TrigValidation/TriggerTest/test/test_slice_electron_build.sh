#!/bin/bash

# art-description: Electron slice TriggerTest on MC
# art-type: build

export SLICE="egamma"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
