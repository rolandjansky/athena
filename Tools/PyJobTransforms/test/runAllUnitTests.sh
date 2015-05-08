#!/bin/bash

PJTUnitTests(){
    exitCode=0
    for test in $(ls ${TestArea}/InstallArea/share/JobTransforms/test/test_transform.py $TestArea/InstallArea/share/JobTransforms/test/test_trf*.py); do
        name="$(basename ${test})"
        echo "Running test "${name}""
        ${test} &> ${name}.test
        if [ ${?} != "0" ]; then 
            echo ""$(date)" "${test}" failed" | tee -a test.fail
            exitCode=1
        else
            echo ""$(date)" "${test}" ok" | tee -a test.ok
        fi
    done
    if [ ${exitCode} != "0" ]; then
        echo "At least one test failed. See summary file test.fail for details."
    else
        echo "All tests passed."
    fi
}

PJTUnitTests