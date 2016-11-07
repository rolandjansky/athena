#! /bin/bash

if [ -z "$ROOTCORE_RELEASE_SERIES" ] 
then 
    echo ROOTCORE_RELEASE_SERIES not set, exiting.....
    return 1
fi

if [ ${ROOTCORE_RELEASE_SERIES} == 21 ]
then 
    run_ut_accessDataAndCpTools $ROOTCORE_TEST_FILE
else 
    echo 'ut test not suitable for release series '$ROOTCORE_RELEASE_SERIES
fi