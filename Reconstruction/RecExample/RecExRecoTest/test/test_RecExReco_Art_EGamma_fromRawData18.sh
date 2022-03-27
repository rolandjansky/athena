#!/bin/sh
#
# art-description: Athena runs egamma (+ calo + ID)  reconstruction from a RAW data18 file
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena
# art-output: *.log   

python $Athena_DIR/python/RecExRecoTest/EGammaReco_RAWData18.py | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"
test_postProcessing_Errors.sh temp.log

