#!/bin/bash

# art-description: Bphysics slice TriggerTest on MC using Jpsimu6mu6 to AOD
# art-type: grid

export NAME="slice_bphysics_aod_grid"
export SLICE="bphysics"
export INPUT="bphysics"
export JOBOPTIONS="testCommonSliceAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
