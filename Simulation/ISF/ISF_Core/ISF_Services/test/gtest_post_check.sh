#!/bin/bash
LOGFILE=${1}
FAILUREPATTERN='\[  FAILED  \].*'
ALIVEPATTERN='\[  PASSED  \].*'

#cat ${LOGFILE}

if ! grep -xq "$ALIVEPATTERN" ${LOGFILE}
then
    echo -e "\nUnable to find gtest signature '${ALIVEPATTERN}' in test output!"
    echo -e "ERROR: Tests failed!\n"
    exit 1
fi

if grep -xq "$FAILUREPATTERN" ${LOGFILE}
then
    echo -e "\nFound failure pattern '${FAILUREPATTERN}' in test output!"
    echo -e "ERROR: Tests failed!\n"
    exit 1
fi
