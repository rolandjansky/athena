#!/bin/sh
#
# art-description: Athena runs Standard ESD pflow reconstruction, PFO linking off, thinning off.
# art-type: grid
# art-include master/Athena
# art-athena-mt: 8
# art-output: AOD.pool.root
# art-output: log_FE_validation.txt

export ATHENA_CORE_NUMBER=8 # set number of cores used in multithread to 8.

athena.py --threads=8  eflowRec/run_ESDPFlowRecoR22.py
rc1=$?

echo "art-result: ${rc1}"
