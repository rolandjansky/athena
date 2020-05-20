#!/bin/bash

# art-description: Trigger MC pp v8 RDO to ESD and AOD test, plus extra tests using these files. With doLVL1Phase1
# art-type: build
# art-include: 21.3/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_ftk_enableLVL1Phase1_rdotoesdaod_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="3"
export INPUT="run3_l1calo"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoESDAOD.py"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags as tf;tf.doLVL1PhaseI.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
source exec_art_triganalysistest_post.sh
