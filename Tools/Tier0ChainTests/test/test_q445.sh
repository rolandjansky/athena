#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

Reco_tf.py \
--AMI=q445 \
--multithreaded \
--maxEvents=500 \
--outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root \
--imf False

rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ $rc1 -eq 0 ]
then
  art.py compare grid --entries 30 "$1" "$2" --mode=semi-detailed --order-trees --ignore-exit-code diff-pool \
  --ignore-leave 'Token' --ignore-leave 'index_ref' --ignore-leave '(.*)_timings\.(.*)' --ignore-leave '(.*)_mems\.(.*)' \
  --ignore-leave '(.*)TrigCostContainer(.*)' --ignore-leave '(.*)HLTNav_Summary_OnlineSlimmed(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF(.*)'  # Ignoring based on ATR-24119 and ATR-24888
  rc2=$?
fi
echo "art-result: $rc2 Diff"

rc3=-9999
if [ $rc1 -eq 0 ]
then
  ArtRef=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_Run3-22.0_references_for_comparison/test_q445
  cat $ArtRef/version.txt
  art.py compare ref --entries 30 . $ArtRef --mode=semi-detailed --order-trees --ignore-exit-code diff-pool \
  --ignore-leave 'Token' --ignore-leave 'index_ref' --ignore-leave '(.*)_timings\.(.*)' --ignore-leave '(.*)_mems\.(.*)' \
  --ignore-leave '(.*)TrigCostContainer(.*)' --ignore-leave '(.*)HLTNav_Summary_OnlineSlimmed(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTF(.*)' \
  --ignore-leave '(.*)TrackParticleAuxContainer_v5_HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF(.*)'  # Ignoring based on ATR-24119 and ATR-24888
  rc3=$?
fi
echo "art-result: $rc3 Diff (fixed reference)"
