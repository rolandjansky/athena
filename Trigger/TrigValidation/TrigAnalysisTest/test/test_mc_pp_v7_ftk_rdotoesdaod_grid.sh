#!/bin/bash

# art-description: Trigger MC pp v7 RDO to ESD and AOD test, plus extra tests using these files
# art-type: grid
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
# art-output: AOD.pool.root.checkFile0
# art-output: AOD.pool.root.checkFiletrigSize.txt
# art-output: *.regtest


export NAME="mc_pp_v7_rdotoesdaod_grid"
export INPUT="ftk"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v7"
export EVENTS="100"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoESDAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
source exec_art_triganalysistest_post.sh
