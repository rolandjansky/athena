#!/bin/bash

# art-description: Bphysics slice TriggerTest on MC using Jpsimu6mu6 to AOD
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="slice_bphysics_build"
export SLICE="bphysics"
export INPUT="bphysics"
export EVENTS="10"
export COST_MONITORING="False"
export EXTRA="jp.AthenaCommonFlags.SkipEvents.set_Value_and_Lock(3);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
