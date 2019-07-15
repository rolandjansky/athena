#!/bin/bash

# art-description: MinBias slice TriggerTest on MinBias MC
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
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

export NAME="slice_minbias_grid"
export SLICE="minbias"
export MENU="Physics_pp_v7"
export INPUT="minbias"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
