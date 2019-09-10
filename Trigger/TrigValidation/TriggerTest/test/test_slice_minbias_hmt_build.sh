#!/bin/bash

# art-description: MinBias Enhanced (High Multiplicity Trigger) slice TriggerTest on MC
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="slice_minbias_hmt_build"
export SLICE="minbiasEnhanced"
export MENU="Physics_pp_v7_primaries"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
