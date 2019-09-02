#!/bin/bash

athena $1 | tee athenaSerial.log
rc=${PIPESTATUS[0]}
echo "art-result: $rc Serial"

test_postProcessing_Errors.sh athenaSerial.log

echo "Creating new threadOne directory"
mkdir threadOne; cd threadOne

athena --threads=1 $1 | tee athenaOneThread.log
rc1=${PIPESTATUS[0]}
echo "art-result: $rc1 OneThread"

test_postProcessing_Errors.sh athenaOneThread.log

var=$(pwd)
echo "The current working directory $var."
ls -als

if [[ $rc -eq 0 ]] && [[ $rc1 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../
 echo "art-result: $? Diff-Serial-OneThread"
fi

cd ../
echo "Creating new threadTwo directory"
mkdir threadTwo; cd threadTwo

athena --threads=2 $1 | tee athenaTwoThreads.log
rc2=${PIPESTATUS[0]}
echo "art-result: $rc2 TwoThreads"

test_postProcessing_Errors.sh athenaTwoThreads.log

if [[ $rc1 -eq 0 ]] && [[ $rc2 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../
 echo "art-result: $? Diff-OneThread-TwoThreads"
fi

cd ../
echo "Creating new threadFive directory"
mkdir threadFive; cd threadFive

athena --threads=5 $1 | tee athenaFiveThreads.log
rc2=${PIPESTATUS[0]}
echo "art-result: $rc5 FiveThreads"

test_postProcessing_Errors.sh athenaFiveThreads.log

if [[ $rc2 -eq 0 ]] && [[ $rc5 -eq  0 ]] 
then
 echo "Compare two directories"
 art.py compare ref --entries 10 --mode=semi-detailed --order-trees --diff-root . ../
 echo "art-result: $? Diff-TwoThreads-FiveThreads"
fi
