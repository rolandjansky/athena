#!/bin/bash

if test "$ROOTCORE_AUTO_UT" == "1" -a "$ROOTCORE_SLOW_UT" != "1"
then
    exit 0
fi

set -e
set -u

FILE="$ROOTCORE_TEST_FILE"
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

quickana_basic "$FILE" output.root optimized
