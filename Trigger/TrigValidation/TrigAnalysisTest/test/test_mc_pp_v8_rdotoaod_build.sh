#!/bin/bash

# art-description: Trigger MC pp v8 RDO to AOD test
# art-type: build
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_rdotoaod_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
