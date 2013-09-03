#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        23 Aug, 2007                            #
#                                                      #
# Description: exports variables required for running  #
#              batch jobs with the "condor" system     #
########################################################


export UNIVERSE="vanilla"
export NOTIFICATION="Never"
export NOTIFY_USER="dufourma@physics.mcgill.ca"
export JOB_EXEC="$CONFIG_PATH/job.sh"
export JOB_ARGS="$CONDOR_SCRIPTS/$JOB_NAME.sh"
export REQUIREMENTS="CPU_Speed >= 1"
export RANK="CPU_Speed"
export IMAGE_SIZE="512 Meg"
export PRIORITY="+20"
export GETENV="True"
export INITIALDIR="/usatlas/groups/trigger/dufourma"
export INPUT="/dev/null"
export EXPERIMENT=\"atlas\"
export JOB_TYPE=\"cas\"
#export RACF_GROUP=\"bilge\"
export RACF_GROUP=\"usatlas\"

export LS_CMD="dq2_ls -fp"
export PATH_PREFIX="dcache:"
#export PATH_PREFIX=""
export CONDOR_SCRIPTS="$OUTPUT_PATH/condor_scripts"
