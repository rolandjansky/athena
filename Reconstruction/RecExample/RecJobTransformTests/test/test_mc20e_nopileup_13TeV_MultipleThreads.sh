#!/bin/sh
#
# art-description: Run mc20e MC setup without pileup and with MT = 8.
# art-output: log.*
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

#Used by HITtoRDO step to configure AthenaMP
declare -i ATHENA_CORE_NUMBER=8

export TRF_ECHO=True;
Reco_tf.py --AMI=r12690 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root  --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV/valid1/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3227/HITS.12560240._000287.pool.root.1  --maxEvents=500 --jobNumber=1 --imf False
RES=$?
echo "art-result: $RES Reco"
if [ $RES -ne 0 ];then
return $RES
fi
