#!/bin/sh
#
# art-description: Run mc20e MC setup without pileup.
# art-output: log.*
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

Reco_tf.py --AMI=r12898 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV/valid1/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3227/HITS.12560240._000287.pool.root.1  --maxEvents=500 --imf False --conditionsTag="OFLCOND-MC16-SDR-RUN2-09"
RES=$?
echo "art-result: $RES Reco"
