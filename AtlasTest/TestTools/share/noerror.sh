#!/usr/bin/bash
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Post-processing script to be used with atlas_add_test to
# check for common error patterns in the log file.

usage() {
    cat <<EOF
Syntax: noerror.sh
Post-processing script that checks the return code of an executable (expected
in \$ATLAS_CTEST_TESTSTATUS) and checks the log files for common error patterns.

Driven by the following environment variables:
   ATLAS_CTEST_TESTNAME              test name
   ATLAS_CTEST_TESTSTATUS            test return code
   [ATLAS_CTEST_LOG_SELECT_PATTERN]  additional error patterns to be considered
   [ATLAS_CTEST_LOG_IGNORE_PATTERN]  lines matching (regex) will be ignored
EOF
}

if [ $# -gt 0 ]; then
    usage
    exit 1
fi

if [ -z "${ATLAS_CTEST_TESTSTATUS}" ]; then
   echo " noerror.sh> ERROR: test exit status is not available (\$ATLAS_CTEST_TESTSTATUS is not set)"
   exit 1
fi

# selection patterns:
errors="\bERROR\b|\bFATAL\b"

if [ -n "${ATLAS_CTEST_LOG_SELECT_PATTERN}" ]; then
    errors="${errors}|${ATLAS_CTEST_LOG_SELECT_PATTERN}"
fi
ignorepatterns=${ATLAS_CTEST_LOG_IGNORE_PATTERN}

joblog=${ATLAS_CTEST_TESTNAME}.log
if [ ! -f "${joblog}" ]; then
    echo " noerror.sh> ERROR: cannot find test log file ${joblog}"
    exit 1
fi

# colors:
if [ -z "${POST_SH_NOCOLOR}" ]; then
    RED="[97;101;1m"
    RESET="[m"
else
    RED=""
    RESET=""
fi

# check log file:
if [ -n "${ATLAS_CTEST_LOG_IGNORE_PATTERN}" ]; then
    matches=`egrep -a "${errors}" ${joblog} | egrep -av "${ATLAS_CTEST_LOG_IGNORE_PATTERN}"`
else
    matches=`egrep -a "${errors}" ${joblog}`
fi

if [ -n "${matches}" ]; then
    echo "$RED noerror.sh> ERROR: Found the following errors in ${joblog}:$RESET"
    echo "${matches}"
    logStatus=1
else
    echo " noerror.sh> No errors found in ${joblog}"
    logStatus=0
fi

# if test itself failed, we return with that failure code:
if [ ${ATLAS_CTEST_TESTSTATUS} != 0 ]; then
    echo "$RED noerror.sh> ERROR: Test ${ATLAS_CTEST_TESTNAME} failed with exit code: ${ATLAS_CTEST_TESTSTATUS}$RESET"
    echo  " noerror.sh> Please check ${PWD}/${joblog}"
    exit ${ATLAS_CTEST_TESTSTATUS}
else
    exit ${logStatus}
fi
