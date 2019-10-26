#!/bin/sh
#
# art-description: Athena (two thread mode only) runs muon segment finding, using the new job configuration system for Run 3, from an ESD file
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

export ATHENA_CORE_NUMBER=8

python -m MuonConfig.MuonSegmentFindingConfig --run --threads=2 -o=ESD_2.pool.root  | tee temp1.log

echo "art-result: ${PIPESTATUS[0]}"


test_postProcessing_Errors.sh temp1.log
