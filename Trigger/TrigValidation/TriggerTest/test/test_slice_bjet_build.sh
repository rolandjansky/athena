#!/bin/bash

# art-description: Bjet slice TriggerTest on MC
# art-type: build

export NAME="slice_bjet_build"
export SLICE="bjet"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
