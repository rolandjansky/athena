#!/bin/bash

# art-description: Jet slice TriggerTest on MC
# art-type: build

export MENU="MC_HI_v4"
export EVENTS="500"
export INPUT="pbpb"

source exec_athena_common_slice.sh
source exec_postprocessing_hist.sh
#!/bin/bash

# art-description: Heavy ion MC v4 TriggerTest on MC
# art-type: grid

export MENU="MC_HI_v4"
export EVENTS="500"
export INPUT="pbpb"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh

return $ATH_EXIT
