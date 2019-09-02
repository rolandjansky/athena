#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: athenaSerial.log
# art-output: athenaOneThread.log
# art-output: athenaTwoThreads.log
# art-output: athenaFiveThreads.log

export ATHENA_CORE_NUMBER=8

test_compare_SerialAndThreadedAthenas.sh eflowRec/run_ESDStandardReco.py 
