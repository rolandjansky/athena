#!/bin/sh
#
# art-description: Athena runs reconstruction job, with no reconstruction algorithms scheduled,  from an ESD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

export ATHENA_CORE_NUMBER=8

athena --threads=8 RecExRecoTest/RecExRecoTest_ART_noAlgs_fromESD.py | tee athenaEightThreads.log
test_postProcessing_Errors.sh athenaEightThreads.log | tee errorsEightThreads.log
