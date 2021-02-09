#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py  --athenaopts="--threads=1" --outputHISTFile=myMergedMonitoring_express_0.root --AMI=f628 --maxEvents=100 --outputESDFile=myESD_express_0.pool.root --outputAODFile=myAOD_express_0.AOD.pool.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/expressstream_input_data18/data18_13TeV.00357750.express_express.merge.RAW._lb0106._SFO-ALL._0001.1 --imf False --conditionsTag='CONDBR2-BLKPA-RUN2-02' --geometryVersion='ATLAS-R2-2016-01-00-01'
echo "art-result: $?"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"
