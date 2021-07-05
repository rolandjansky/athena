#!/bin/sh
#
# art-description: Run mc20e MC setup without pileup and with stdcmalloc enabled.
# art-output: log.*
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

#Unset ATHENA_CORE_NUMBER to bypass ATLASJT-418                                                                                                                                                                                                                             
unset ATHENA_CORE_NUMBER
unset ATHENA_PROC_NUMBER

export TRF_ECHO=True;
Reco_tf.py --AMI=r12690 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV/valid1/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3227/HITS.12560240._000287.pool.root.1  --maxEvents=500 --jobNumber=1 --imf False --athenaopts "all:--stdcmalloc" "RAWtoESD:--threads=1" "ESDtoAOD:--threads=1" 
RES=$?
echo "art-result: $RES Reco"
