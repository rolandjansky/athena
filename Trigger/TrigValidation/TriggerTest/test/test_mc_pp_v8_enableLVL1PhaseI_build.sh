#!/bin/bash

# art-description: MC v8 TriggerTest on MC
# art-type: build
# art-include: 21.3/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_enableLVL1PhaseI_build"
export MENU="MC_pp_v8"
export EVENTS="5"
export INPUT="run3_l1calo"
export COST_MONITORING="False"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags as tf;tf.doLVL1PhaseI.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
