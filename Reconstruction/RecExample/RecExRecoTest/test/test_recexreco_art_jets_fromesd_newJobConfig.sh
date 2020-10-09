#!/bin/sh
#
# art-description: Athena runs jet reconstruction, using the new job configuration for Run 3, from an ESD file
# art-type: local
# art-athena-mt
# art-include: master/Athena

python $WorkDir_DIR/jobOptions/JetRecConfig/test_StandardSmallRJets.py  --filesIn /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root -n 50 | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp.log
