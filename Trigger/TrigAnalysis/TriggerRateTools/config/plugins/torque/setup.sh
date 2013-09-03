#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        10 Aug, 2007                            #
#                                                      #
# Description: exports variables required for running  #
#              batch jobs with the "torque" system     #
########################################################


export JOB_CMD="qsub" #Main command to submit a job
export COMMON_OPTIONS=" -q long "
export PASS_VARS_FLAG=" -v "
export ERROR_FLAG=" -e " #option used to specify the output path to the error (stderr) file
export OUTPUT_FLAG=" -o " #option used to specify the output path to the output (stdout) file
export NAME_FLAG=" -N " #option used to specify the output path to the output (stdout) file
export SUBMIT_SCRIPT="$CONFIG_PATH/job.sh"
