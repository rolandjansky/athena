#!/bin/bash

# art-description: MC v7 no prescales TriggerTest on FTK MC to AOD
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig_*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: costMonitoring_*
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*

export NAME="mc_pp_v7_nops_aod_ftk_grid"
export MENU="MC_pp_v7_no_prescale"
export INPUT="ftk"
export JOBOPTIONS="TriggerTest/testCommonSliceAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
