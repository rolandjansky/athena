#!/bin/sh
#
# art-description: Athena runs topoclustering, particle flow and particle flow jet reconstruction from an ESD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: serial
# art-output: threadOne
# art-output: threadEight

export ATHENA_CORE_NUMBER=8

test_compare_SerialAndThreadedAthenas.sh eflowRec/run_ESDPFlowRecoR22_withLinking.py
