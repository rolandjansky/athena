#!/bin/bash

# art-description: MinBias Enhanced (High Multiplicity Trigger) slice TriggerTest on MC
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
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

export NAME="slice_minbias_hmt_grid"
export SLICE="minbiasEnhanced"
export MENU="Physics_pp_v7"
export EVENTS="50"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
