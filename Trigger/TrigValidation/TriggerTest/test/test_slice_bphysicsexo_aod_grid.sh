#!/bin/bash

# art-description: Bphysics and muon slice TriggerTest on MC using Wtaunu_3mu to AOD
# art-type: grid

export NAME="slice_bphysicsexo_aod_grid"
export SLICE="mubphysics"
export INPUT="mubphysics"
export JOBOPTIONS="testCommonSliceAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
