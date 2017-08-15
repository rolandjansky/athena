#!/bin/bash

# art-description: MC v7 no prescales TriggerTest on FTK MC to AOD
# art-type: grid

export MENU="MC_pp_v7_no_prescale"
export INPUT="ftk"
export JOBOPTIONS="testCommonSliceAthenaTrigRDOtoAOD.py"


source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
