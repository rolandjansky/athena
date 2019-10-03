#!/bin/sh
#
# art-description: Athena (one thread mode only) runs muon track building, using the new job configuration system for Run 3, from an ESD file
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

export ATHENA_CORE_NUMBER=8

python -m MuonConfig.MuonTrackBuildingConfig --run --threads=1 -o=ESD_1.pool.root  | tee temp1.log
echo "art-result: ${PIPESTATUS[0]}"

python -m MuonConfig.MuonTrackBuildingConfig --run --threads=8 -o=ESD_8.pool.root  | tee temp2.log
echo "art-result: ${PIPESTATUS[0]}"

acmd diff-root --order-trees -mode=semi-detailed ESD_1.pool.root ESD_8.pool.root 

test_postProcessing_Errors.sh temp1.log
test_postProcessing_Errors.sh temp2.log
