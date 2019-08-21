#!/bin/bash

# art-description: Trigger MC pp v7 RDO to ESD and AOD test, plus extra tests using these files
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v7_rdotoesdaod_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v7"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoESDAOD.py"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
source exec_art_triganalysistest_post.sh
