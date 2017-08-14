#!/bin/bash

# art-description: Tau slice TriggerTest on MC
# art-type: grid

export NAME="slice_tau_grid"
export SLICE="tau"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
