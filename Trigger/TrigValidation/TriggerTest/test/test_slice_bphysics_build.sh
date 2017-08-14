#!/bin/bash

# art-description: Bphysics slice TriggerTest on MC using Jpsimu6mu6 to AOD
# art-type: build

export NAME="slice_bphysics_build"
export SLICE="bphysics"
export INPUT="bphysics"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
