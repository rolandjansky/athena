#!/bin/bash

# art-description: Bphysics slice TriggerTest on MC using Jpsimu6mu6 to AOD
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: costMonitoring_*
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: TotalEventsProcessed.txt
# art-output: *.regtest.new

export NAME="slice_bphysics_build"
export SLICE="bphysics"
export INPUT="bphysics"
export EVENTS="10"
export COST_MONITORING="False"
export EXTRA="jp.AthenaCommonFlags.SkipEvents.set_Value_and_Lock(3);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
