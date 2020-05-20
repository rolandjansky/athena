#!/bin/bash

# art-description: MC v8 TriggerTest on MC
# art-type: grid
# art-include: 21.3/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: costMonitoring_*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

export NAME="mc_pp_v8_enableLVL1PhaseI_grid"
export MENU="MC_pp_v8"
export EVENTS="500"
export INPUT="run3_l1calo"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags as tf;tf.doLVL1PhaseI.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
