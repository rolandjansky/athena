#!/bin/bash
# art-description: Standard test for 2015 data
# art-type: grid
# art-include: master/Athena
# art-output: physval*.root
# art-output: *.xml
# art-output: dcube*

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }


lastref_dir=last_results
artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
inputBS=${artdata}/RecJobTransformTests/data15_13TeV.00283429.physics_Main.daq.RAW._lb0154._SFO-1._0001.data 
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/dcube/config/IDPVMPlots_R22.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/ReferenceHistograms/physval_data15_1000evt_reco_r22.root"

# Reco step based on test InDetPhysValMonitoring ART setup from Josh Moss.

run  Reco_tf.py \
  --inputBSFile "$inputBS" \
  --maxEvents 1000 \
  --autoConfiguration everything \
  --conditionsTag="CONDBR2-BLKPA-2016-14" \
  --outputAODFile   physval.AOD.root \
  --outputNTUP_PHYSVALFile physval.ntuple.root \
  --steering        doRAWtoALL \
  --checkEventCount False \
  --ignoreErrors    True \
  --valid           True \
  --validationFlags doInDet \
  --preExec 'from InDetRecExample.InDetJobProperties import InDetFlags; \
  InDetFlags.doSlimming.set_Value_and_Lock(False); rec.doTrigger.set_Value_and_Lock(False); \
  from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; \
  InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True); \
  InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(False); \
  InDetPhysValFlags.doValidateGSFTracks.set_Value_and_Lock(False); \
  InDetPhysValFlags.doPhysValOutput.set_Value_and_Lock(True); \
  rec.doDumpProperties=True; rec.doCalo=True; rec.doEgamma=True; \
  rec.doForwardDet=False; rec.doInDet=True; rec.doJetMissingETTag=True; \
  rec.doLArg=True; rec.doLucid=True; rec.doMuon=True; rec.doMuonCombined=True; \
  rec.doSemiDetailedPerfMon=True; rec.doTau=True; rec.doTile=True; \
  from ParticleBuilderOptions.AODFlags import AODFlags; \
  AODFlags.ThinGeantTruth.set_Value_and_Lock(False);  \
  AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False); \
  AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False);\
  AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False) '
rec_tf_exit_code=$?
echo "art-result: $rec_tf_exit_code reco"

if [ $rec_tf_exit_code -eq 0 ]  ;then
  echo "download latest result"
  run art.py download --user=artprod --dst="$lastref_dir" "$ArtPackage" "$ArtJobName"
  run ls -la "$lastref_dir"

  echo "compare with R22 with nightly build at 2020-12-05"
  $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube \
    -c ${dcubeXml} \
    -r ${dcubeRef} \
    physval.ntuple.root
  echo "art-result: $? plots"
  
  echo "compare with last build"
  $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube_last \
    -c ${dcubeXml} \
    -r ${lastref_dir}/physval.ntuple.root \
    physval.ntuple.root
  echo "art-result: $? plots"
fi

