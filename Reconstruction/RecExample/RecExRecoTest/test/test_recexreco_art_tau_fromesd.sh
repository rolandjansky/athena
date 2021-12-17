#!/bin/sh
#
# art-description: Athena runs tau reconstruction from an ESD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-output: *.log   

export ATHENA_CORE_NUMBER=8

athena --threads=8 tauRec/tau_standalone_LCTopo_ESDtoAOD.py | tee athenaEightThreads.log
echo "art-result: ${PIPESTATUS[0]}"
test_postProcessing_Errors.sh athenaEightThreads.log | tee errorsEightThreads.log
