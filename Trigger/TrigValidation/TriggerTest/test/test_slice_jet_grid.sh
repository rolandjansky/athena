#!/bin/bash

# art-description: Jet slice TriggerTest on MC
# art-type: grid

export SLICE="jet"


source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
