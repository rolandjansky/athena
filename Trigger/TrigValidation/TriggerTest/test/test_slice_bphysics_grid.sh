#!/bin/bash

# art-description: Bphysics slice TriggerTest on MC using Jpsimu6mu6 to AOD
# art-type: grid
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

export NAME="slice_bphysics_grid"
export SLICE="bphysics"
export INPUT="bphysics"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
