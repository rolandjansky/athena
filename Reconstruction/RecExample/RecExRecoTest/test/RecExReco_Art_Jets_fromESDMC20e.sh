#!/bin/sh
#
# art-description: Athena runs jet reconstruction, using the new job configuration for Run 3, from an ESD file
# art-type: grid
# art-athena-mt
# art-include: master/Athena
# art-output: *.log   

test_StandardJets.py  --filesIn /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root -n 50 | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"
test_postProcessing_Errors.sh temp.log

