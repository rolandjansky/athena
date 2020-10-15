#!/bin/sh
#
# art-description: Run PhysicaValidation Example code. Uses AOD produced from test_mcLatest_13teV.sh on 7 October 2020.
# art-output: log.*
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

export TRF_ECHO=True;
Reco_tf.py --inputAODFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mcLatest/mcLatest_13TeV_AOD.pool.root --outputNTUP_PHYSVALFile physval.root  --validationFlags doExample
RES=$?
echo "art-result: $RES Reco"
if [ $RES -ne 0 ];then
return $RES
fi
