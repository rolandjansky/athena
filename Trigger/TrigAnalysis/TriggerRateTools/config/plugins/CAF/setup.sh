#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        5 Nov, 2008                             #
#                                                      #
# Description: exports variables required for running  #
#              batch jobs with the "CAF" system        #
########################################################


export JOB_CMD="bsub" #Main command to submit a job
export COMMON_OPTIONS=""
export QUEUE_FLAG=" -q "
export QUEUE_NAME="atlastrig"
export ERROR_FLAG=" -e " #option used to specify the output path to the error (stderr) file
export OUTPUT_FLAG=" -o " #option used to specify the output path to the output (stdout) file
export NAME_FLAG=" -J " #option used to specify the output path to the output (stdout) file
export SUBMIT_SCRIPT="$CONFIG_PATH/job.sh"

export LS_CMD="nsls"
export PATH_PREFIX="rfio:"
