#!/bin/bash

# art-description: Trigger MC pp v8 RDO to BS and BS to ESD test
# art-type: grid
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
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

export NAME="mc_pp_v8_rdotobstoesd_grid"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="100"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoBS.py"

source exec_athena_art_trigger_validation.sh
athena.py -c "jp.AthenaCommonFlags.BSRDOInput=['raw.data']" TrigAnalysisTest/testAthenaTrigBStoESD.py | tee ${JOB_LOG%%.*}.${JOB_LOG#*.}
echo "art-result: ${PIPESTATUS[0]} athena.BStoESD"

source exec_art_triggertest_post.sh
