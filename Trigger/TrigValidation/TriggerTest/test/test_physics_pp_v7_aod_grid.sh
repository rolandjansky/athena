#!/bin/bash

# art-description: Physics v7 TriggerTest on MC to AOD
# art-type: grid

export MENU="Physics_pp_v7"
export EVENTS="500"
export JOBOPTION="testCommonSliceAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
