#!/bin/bash

# art-description: Trigger Physics pp v7 RDO to BS and BS to AOD test
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
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

export NAME="physics_pp_v7_rdotobstoesd_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="Physics_pp_v7"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoBS.py"

source exec_athena_art_trigger_validation.sh

echo $(date "+%FT%H:%M %Z")"     Running checklog for ${JOB_LOG}"
timeout 5m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG}  2>&1 | tee checklog_BS.log

echo "art-result: ${PIPESTATUS[0]} CheckLog BS"

export JOB_LOG="athena_ESD.log"

athena.py -c "jp.AthenaCommonFlags.BSRDOInput=['raw.data']" TrigAnalysisTest/testAthenaTrigBStoESD.py | tee ${JOB_LOG%%.*}.${JOB_LOG#*.}
echo "art-result: ${PIPESTATUS[0]} ${JOB_LOG%%.*}"


source exec_art_triggertest_post.sh


