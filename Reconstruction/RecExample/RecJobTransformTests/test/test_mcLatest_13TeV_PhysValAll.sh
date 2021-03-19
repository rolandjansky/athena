#!/bin/sh
#
# art-description: Run PhysicaValidation code from all domains. Uses AOD produced from test_mcLatest_13teV.sh on 7 October 2020.
# art-output: log.*
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

export TRF_ECHO=True;
Reco_tf.py --inputAODFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc_CPSamples_Round1_2021/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.AOD.e6337_e5984_s3170_r12399_r12253_r12399/AOD.24234434._000011.pool.root.1 --outputNTUP_PHYSVALFile physval.root  --validationFlags 'doExample,doMET,doPFlow,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult'
RES=$?
echo "art-result: $RES Reco"
if [ $RES -ne 0 ];then
return $RES
fi
