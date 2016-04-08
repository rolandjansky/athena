#!/bin/bash

whatToDo=$1

if test "$whatToDo" = "-h" || test "$whatToDo" = "--help";then
    echo "./run.sh --jobs ==> to run jobs"
    echo "./run.sh --tests ==> to run tests (assumes you have a testsuites/ dir in the current dir)"
    echo "./run.sh --tests -d <path_to_testsuites_dir> ==> to run tests with different testsuites dir"
else
    if test "$whatToDo" != "--tests" && test "$whatToDo" != "--jobs"; then
	echo "Unknown argument", $whatToDo, "must be one of --jobs or --tests.";
    fi
fi

export PYTHONPATH=`pwd`/src:$RTTSrcDir:$PYTHONPATH

if test "$whatToDo" = "--jobs";then
    python run/runSuites.py
elif test "$whatToDo" = "--tests";then
    if [ $# -eq 1 ]; then
	python run/runTests.py -d testsuites	
    else
	shift;
	python run/runTests.py $@	
    fi
fi

