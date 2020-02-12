#!/bin/bash

# art-description: MinBias slice TriggerTest on MinBias MC
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="slice_minbias_build"
export SLICE="minbias"
export MENU="Physics_pp_v7"
export EVENTS="10"
export INPUT="minbias"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
