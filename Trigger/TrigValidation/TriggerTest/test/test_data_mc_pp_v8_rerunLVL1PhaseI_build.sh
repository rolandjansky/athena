#!/bin/bash

# art-description: MC v8 TriggerTest on data with rerunLVL1PhaseI=True
# art-type: build
# art-include: 21.3/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="data_mc_pp_v8_rerunLVL1PhaseI_build"
export MENU="MC_pp_v8"
export EVENTS="100"
export INPUT="data"
export EXTRA="rerunLVL1PhaseI=True;"

# The flag below skips HLTTE and L1 counts checks, but keeps the HLTChain counts check.
# This is needed because we run on data without rerunLvl1, so we only produce HLTChain counts.
export SKIP_CHAIN_DUMP=2

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
