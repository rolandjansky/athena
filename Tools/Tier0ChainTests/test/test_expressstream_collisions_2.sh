#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8                                                                                                                                    

Reco_tf.py \
--AMI=f628 \
--athenaopts="--threads=8" \
--maxEvents=50 \
--ignoreErrors=False \
--inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/expressstream_input_data18/data18_13TeV.00357750.express_express.merge.RAW._lb0091._SFO-ALL._0001.1 \
--conditionsTag='CONDBR2-BLKPA-RUN2-09' \
--geometryVersion='ATLAS-R2-2016-01-00-01' \
--outputESDFile=myESD_express_0.pool.root --outputAODFile=myAOD_express_0.AOD.pool.root --outputHISTFile=myMergedMonitoring_express_0.root --imf False


rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

