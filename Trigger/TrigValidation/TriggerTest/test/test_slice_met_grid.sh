#!/bin/bash

# art-description: MET slice TriggerTest on MC
# art-type: grid

export SLICE="met"


source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
