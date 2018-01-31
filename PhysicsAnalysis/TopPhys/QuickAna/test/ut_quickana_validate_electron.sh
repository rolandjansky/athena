#!/bin/bash

if test "$ROOTCORE_AUTO_UT" == "1" -a "$ROOTCORE_SLOW_UT" != "1"
then
    exit 0
fi

set -e
set -u

FILE="$ASG_TEST_FILE_MC"
if test "$FILE" == ""
then
    echo no test file set
    exit 1
fi
if test \! -f "$FILE"
then
    echo file does not exist: $FILE
    exit 1
fi

OUTPUT_DIR=output.$$
rm -rf $OUTPUT_DIR
quickana_validate --qa-no-default-def --qa-electron-def default --el-input-file "$FILE" $OUTPUT_DIR
