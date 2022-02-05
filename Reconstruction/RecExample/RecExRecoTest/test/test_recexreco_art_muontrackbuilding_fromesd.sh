#!/bin/sh
#
# art-description: Athena (eight thread mode only) runs muon track building, using the new job configuration system for Run 3, from an ESD file
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena
# art-output: *.log   

export ATHENA_CORE_NUMBER=8

python -m MuonConfig.MuonTrackBuildingConfig --run --threads=8 -o=ESD_8.pool.root  | tee temp1.log
echo "art-result: ${PIPESTATUS[0]}"
test_postProcessing_Errors.sh temp1.log
