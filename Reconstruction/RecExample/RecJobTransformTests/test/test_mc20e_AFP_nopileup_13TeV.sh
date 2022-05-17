#!/bin/sh
#
# art-description: Run mc20e MC setup, with APF, without pileup.
# art-output: log.*
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena
# art-include: 22.0/Athena

Reco_tf.py --AMI=r12898 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/user.ladamczy/user.ladamczy.mc15_13TeV.860102.SuperChicPy8_gg_jj_CEP_70_new.evgen.HITS.e8419.v1_EXT0/user.ladamczy.28711500.EXT0._000009.HITS.pool.root  --maxEvents=500 --imf False --conditionsTag="OFLCOND-MC16-SDR-RUN2-09"
RES=$?
echo "art-result: $RES Reco"
