#!/bin/bash

# art-description: Muon slice TriggerTest on MC
# art-type: grid

export NAME="slice_muon_grid"
export SLICE="muon"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
