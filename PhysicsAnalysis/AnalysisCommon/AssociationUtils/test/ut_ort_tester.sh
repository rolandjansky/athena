#!/bin/bash

if test "$ROOTCORE_AUTO_UT" == "1" -a "$ROOTCORE_SLOW_UT" != "1"
then
    exit 0
fi

set -e
set -u

OverlapRemovalTester -i $ASG_TEST_FILE_MC -n 50
