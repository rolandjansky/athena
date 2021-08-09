#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-athena-mt: 8

Reco_tf.py \
--AMI=f1089 \
--athenaopts='--threads=8' \
--conditionsTag 'all:CONDBR2-BLKPA-RUN2-06' \
--maxEvents=250 \
--outputESDFile=myESD_Main.pool.root --outputAODFile=myAOD_Main.AOD.pool.root --outputHISTFile=myMergedMonitoring_Main.root \
--ignoreErrors=False \
--inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00348618.physics_Main.daq.RAW._lb0220._SFO-1._0001.data



rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

