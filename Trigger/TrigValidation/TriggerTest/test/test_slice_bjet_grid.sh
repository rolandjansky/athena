#!/bin/bash

# art-description: Bjet slice TriggerTest on MC
# art-type: grid

export NAME="slice_bjet_grid"
export SLICE="bjet"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
