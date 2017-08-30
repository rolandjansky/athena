#!/bin/bash

# art-description: MinBias slice TriggerTest on MinBias MC
# art-type: grid

export SLICE="minbias"
export MENU="Physics_pp_v7"
export INPUT="minbias"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
