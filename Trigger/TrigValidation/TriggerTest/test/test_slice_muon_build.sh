#!/bin/bash

# art-description: Muon slice TriggerTest on MC
# art-type: build

export NAME="slice_muon_build"
export SLICE="muon"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
