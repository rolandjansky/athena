#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        10 Aug, 2007                            #
#                                                      #
# Description: submits jobs to the "torque" system     #
########################################################

SUBMIT_JOB=$JOB_CMD

if [ -n "$COMMON_OPTIONS" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$COMMON_OPTIONS"
fi  

if [ -n "$PASS_VARS_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$PASS_VARS_FLAG""FILE=$FILE,ROOT_FILE=$ROOT_FILE,CONFIG_FILE=$CONFIG_FILE,RUN_DIR_SPECIFIC=$RUN_DIR_SPECIFIC"
fi

if [ -n "$ERROR_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$ERROR_FLAG""$OUTPUT_PATH/log/$JOB_NAME.log"
fi

if [ -n "$OUTPUT_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$OUTPUT_FLAG""$OUTPUT_PATH/out/$JOB_NAME.out"
fi

if [ -n "$NAME_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$NAME_FLAG""$JOB_NAME"
fi

if [ -n "$SUBMIT_SCRIPT" ]; then
    if [ $1 == 3 ]; then
        SUBMIT_JOB="$SUBMIT_JOB $JOB_EXEC"
    else
        SUBMIT_JOB="$SUBMIT_JOB $SUBMIT_SCRIPT"
    fi
fi

$SUBMIT_JOB
