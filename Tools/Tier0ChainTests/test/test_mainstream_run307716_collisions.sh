#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

# disabled HLTMon in preExec as recommended in https://its.cern.ch/jira/browse/ATR-22947

Reco_tf.py \
--AMI=f741 \
--preExec 'r2a:from InDetPrepRawDataToxAOD.SCTxAODJobProperties import SCTxAODFlags;SCTxAODFlags.Prescale.set_Value_and_Lock(50);from AthenaMonitoring.DQMonFlags import DQMonFlags; DQMonFlags.doHLTMon=False' \
--maxEvents=375 \
--conditionsTag=CONDBR2-BLKPA-RUN2-03 \
--geometryVersion='ATLAS-R2-2016-01-00-01' \
--outputESDFile=myESD_Main_0.pool.root --outputAODFile=myAOD_Main_0.AOD.pool.root --outputHISTFile=myMergedMonitoring_Main_0.root \
--inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0220._SFO-1._0001.data,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0221._SFO-1._0001.data \
--ignoreErrors=False \
--imf False


rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

