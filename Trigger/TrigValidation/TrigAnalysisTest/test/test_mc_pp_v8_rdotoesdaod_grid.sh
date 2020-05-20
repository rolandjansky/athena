#!/bin/bash

# art-description: Trigger MC pp v8 RDO to ESD and AOD test, plus extra tests using these files. 
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
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

export NAME="mc_pp_v8_rdotoesdaod_grid"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="50"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoESDAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
source exec_art_triganalysistest_post.sh
