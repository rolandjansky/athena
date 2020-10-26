#!/bin/sh
#
# art-description: Athena runs Standard ESD reconstruction including FlowElements, which are not in the default run
# art-type: grid
# art-include master/Athena
# art-athena-mt: 8
# art-output: AOD.pool.root
# art-output: log_FE_validation.txt

export ATHENA_CORE_NUMBER=8 # set number of cores used in multithread to 8.

athena.py eflowRec/run_ESDStandardReco_FlowElements.py 
rc1=$?
python eflowRec/validateFlowElements.py --f AOD.pool.root &> log_FE_validation.txt
rc2=$?
echo "art-result: ${rc1} ESDtoAOD_FE"
echo "art-result: ${rc2} AOD_readout"
