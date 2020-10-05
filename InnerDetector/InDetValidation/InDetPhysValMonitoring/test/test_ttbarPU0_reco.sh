#!/bin/bash
# art-description: Standard test for Run2 with ttbar input, PU=0
# art-type: grid
# art-include: master/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }


lastref_dir=last_results
artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
inputRDO=${artdata}/InDetPhysValMonitoring/inputs/OUT.RDO_ttbar_PU0.pool.root 
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/dcube/config/IDPVMPlots_R22.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/ReferenceHistograms/physval_ttbarPU0_reco_r22.root"

# Reco step based on test InDetPhysValMonitoring ART setup from Josh Moss.
run Reco_tf.py \
  --inputRDOFile   "$inputRDO" \
  --outputAODFile   physval.AOD.root \
  --outputNTUP_PHYSVALFile physval.ntuple.root \
  --steering        doRAWtoALL \
  --checkEventCount False \
  --ignoreErrors    True \
  --maxEvents       1000 \
  --valid           True \
  --validationFlags doInDet \
  --preExec 'from InDetRecExample.InDetJobProperties import InDetFlags; \
  InDetFlags.doSlimming.set_Value_and_Lock(False); rec.doTrigger.set_Value_and_Lock(False); \
  from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; \
  InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True); \
  InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(False); \
  InDetPhysValFlags.doValidateGSFTracks.set_Value_and_Lock(False); \
  InDetPhysValFlags.doPhysValOutput.set_Value_and_Lock(True); \
  rec.doDumpProperties=True; rec.doCalo=False; rec.doEgamma=False; \
  rec.doForwardDet=False; rec.doInDet=True; rec.doJetMissingETTag=False; \
  rec.doLArg=False; rec.doLucid=False; rec.doMuon=False; rec.doMuonCombined=False; \
  rec.doSemiDetailedPerfMon=True; rec.doTau=False; rec.doTile=False;\
  from ParticleBuilderOptions.AODFlags import AODFlags;\
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

  echo "compare with R21"
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

