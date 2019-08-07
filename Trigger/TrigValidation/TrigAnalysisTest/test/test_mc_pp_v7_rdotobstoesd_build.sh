#!/bin/bash

# art-description: Trigger MC pp v7 RDO to BS and BS to ESD test
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v7_rdotobstoesd_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v7"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoBS.py"

source exec_athena_art_trigger_validation.sh

#the ConditionsTag should match what was used in the RDOtoBS step
athena.py -c "jp.AthenaCommonFlags.BSRDOInput=['raw.data'];from AthenaCommon.GlobalFlags import globalflags;globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-MC16-SDR-25')" TrigAnalysisTest/testAthenaTrigBStoESD.py | tee ${JOB_LOG%%.*}.BStoESD.${JOB_LOG#*.}
echo "art-result: ${PIPESTATUS[0]} athena.BStoESD"

source exec_art_triggertest_post.sh
