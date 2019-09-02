#!/bin/sh
#
# art-description: Athena runs btagging from an ESD file
# art-type: grid
# art-include: master/Athena

athena BTagging/RecExRecoTest_ART_btagging_fromESD.py | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp.log
