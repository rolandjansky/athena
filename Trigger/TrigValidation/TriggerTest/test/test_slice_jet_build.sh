#!/bin/bash

# art-description: Jet slice TriggerTest on MC
# art-type: build

export NAME="slice_jet_build"
export SLICE="jet"
export EVENTS="10"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
