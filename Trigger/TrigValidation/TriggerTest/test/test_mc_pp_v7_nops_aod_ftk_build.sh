#!/bin/bash

# art-description: MC v7 no prescales TriggerTest on FTK MC to AOD
# art-type: build

export NAME="mc_pp_v7_nops_aod_ftk_build"
export MENU="MC_pp_v7_no_prescale"
export INPUT="ftk"
export JOBOPTIONS="testCommonSliceAthenaTrigRDOtoAOD.py"
export EVENTS="5"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
