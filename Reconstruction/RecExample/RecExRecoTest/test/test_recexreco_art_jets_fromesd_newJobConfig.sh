#!/bin/sh
#
# art-description: Athena runs jet reconstruction, using the new job configuration for Run 3, from an ESD file
# art-type: grid
# art-athena-mt
# art-include: master/Athena

python $Athena_DIR/jobOptions/JetRecConfig/test_StandardSmallRJets.py  --filesIn /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3170_r12310_r12253_r12310/ESD.23850840._000295.pool.root.1 -n 50 | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp.log
