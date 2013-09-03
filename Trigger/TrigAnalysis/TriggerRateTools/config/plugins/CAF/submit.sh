#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        5 Nov, 2008                             #
#                                                      #
# Description: submits jobs to the "CAF" system        #
########################################################

SUBMIT_JOB=$JOB_CMD

if [ -n "$COMMON_OPTIONS" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$COMMON_OPTIONS"
fi  

if [ -n "$QUEUE_FLAG" ]; then
    if [ -n "$QUEUE_NAME" ]; then
        SUBMIT_JOB="$SUBMIT_JOB$QUEUE_FLAG$QUEUE_NAME"
    else
        if [ $COUNT -lt $((MAX_COUNT/3)) ]; then
            SUBMIT_JOB="$SUBMIT_JOB$QUEUE_FLAG atlastrig"
        else
            SUBMIT_JOB="$SUBMIT_JOB$QUEUE_FLAG atlascatlong"
        fi
    fi
fi  

if [ -n "$ERROR_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$ERROR_FLAG""$OUTPUT_PATH/log/$JOB_NAME.log"
fi

if [ -n "$OUTPUT_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$OUTPUT_FLAG""$OUTPUT_PATH/out/$JOB_NAME.out"
fi

if [ -n "$NAME_FLAG" ]; then
    SUBMIT_JOB="$SUBMIT_JOB$NAME_FLAG""\"$JOB_NAME\""
fi

if [ -n "$SUBMIT_SCRIPT" ]; then
    if [ $1 == 3 ]; then
        SUBMIT_JOB="$SUBMIT_JOB $JOB_EXEC"
    else
        SUBMIT_JOB="$SUBMIT_JOB $SUBMIT_SCRIPT"
    fi
fi

$SUBMIT_JOB
