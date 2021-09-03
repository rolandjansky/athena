#!/bin/sh
#
# art-description: RecoTrf with beamType=collisions
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py \
--AMI=f741 \
--maxEvents=375 \
--conditionsTag=CONDBR2-BLKPA-RUN2-07 \
--geometryVersion='ATLAS-R2-2016-01-00-01' \
--inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0220._SFO-1._0001.data,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0221._SFO-1._0001.data \
--outputESDFile=myESD_Main_2.pool.root --outputAODFile=myAOD_Main_2.AOD.pool.root \
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

