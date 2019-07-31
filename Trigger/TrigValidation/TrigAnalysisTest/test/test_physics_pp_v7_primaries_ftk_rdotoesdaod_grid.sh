#!/bin/bash

# art-description: Trigger v7 primaries RDO to ESD and AOD test, plus extra tests using these files. With FTK
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

export NAME="physics_pp_v7_primaries_ftk_rdotoesdaod_grid"
export INPUT="ftk"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="Physics_pp_v7_primaries"
export EVENTS="100"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoESDAOD.py"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.doFTK=True;from RecExConfig.RecFlags import rec;rec.doFTK.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
source exec_art_triganalysistest_post.sh
