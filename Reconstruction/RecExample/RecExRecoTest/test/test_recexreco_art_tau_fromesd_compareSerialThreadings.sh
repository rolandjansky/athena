#!/bin/sh
#
# art-description: Athena runs tau reconstruction from an ESD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: serial
# art-output: threadOne
# art-output: threadTwo
# art-output: threadFive

export ATHENA_CORE_NUMBER=8

test_compare_SerialAndThreadedAthenas.sh tauRec/tau_standalone_LCTopo_ESDtoAOD.py
