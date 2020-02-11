#!/bin/bash

# art-description: Trigger MC pp v8 RDO to AOD test FTK
# art-type: build
# art-include: 21.3/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_ftk_rdotoaod_build"
export INPUT="ftk"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoAOD.py"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.doFTK=True;from RecExConfig.RecFlags import rec;rec.doFTK.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
