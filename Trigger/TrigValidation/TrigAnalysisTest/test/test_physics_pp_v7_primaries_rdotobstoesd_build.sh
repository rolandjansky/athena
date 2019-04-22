#!/bin/bash

# art-description: Trigger v7 primaries RDO to BS and BS to ESD test
# art-type: build
# art-include: master/Athena
# art-output: *check*
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: TotalEventsProcessed.txt
# art-output: *.regtest.new

export NAME="physics_pp_v7_primaries_rdotobstoesd_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="Physics_pp_v7_primaries"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoBS.py"

source exec_athena_art_trigger_validation.sh
athena.py -c "jp.AthenaCommonFlags.BSRDOInput=['raw.data']" TrigAnalysisTest/testAthenaTrigBStoESD.py | tee ${JOB_LOG%%.*}.BStoESD.${JOB_LOG#*.}
echo "art-result: ${PIPESTATUS[0]} ${JOB_LOG%%.*}BStoESD"

source exec_art_triggertest_post.sh
