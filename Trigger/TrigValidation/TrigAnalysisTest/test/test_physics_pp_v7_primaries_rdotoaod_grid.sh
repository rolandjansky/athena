#!/bin/bash

# art-description: Trigger v7 primaries RDO to AOD test
# art-type: grid
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
# art-output: AOD.pool.root.checkFile0
# art-output: AOD.pool.root.checkFiletrigSize.txt
# art-output: *.regtest.new

export NAME="physics_pp_v7_primaries_rdotoaod_grid"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="Physics_pp_v7_primaries"
export EVENTS="100"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
