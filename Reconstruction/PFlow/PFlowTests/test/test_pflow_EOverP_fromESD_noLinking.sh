#!/bin/sh
#
# art-description: Athena runs EOverP ESD pflow reconstruction, PFO linking off
# art-type: grid
# art-include: 21.9/Athena
# art-output: AOD.pool.root

athena.py eflowRec/run_ESDPFlowRecoR21_9_EOverP.py
rc1=$?

echo "art-result: ${rc1}"

