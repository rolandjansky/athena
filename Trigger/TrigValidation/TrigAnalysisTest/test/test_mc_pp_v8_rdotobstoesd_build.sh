#!/bin/bash

# art-description: Trigger MC pp v8 RDO to BS and BS to ESD test
# art-type: build
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_rdotobstoesd_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoBS.py"

source exec_athena_art_trigger_validation.sh
athena.py -c "jp.AthenaCommonFlags.BSRDOInput=['raw.data']" TrigAnalysisTest/testAthenaTrigBStoESD.py | tee ${JOB_LOG%%.*}.BStoESD.${JOB_LOG#*.}
echo "art-result: ${PIPESTATUS[0]} athena.BStoESD"

source exec_art_triggertest_post.sh
