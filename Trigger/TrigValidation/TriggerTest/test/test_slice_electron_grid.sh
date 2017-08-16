#!/bin/bash

# art-description: Electron slice TriggerTest on MC
# art-type: grid

export SLICE="egamma"


source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
