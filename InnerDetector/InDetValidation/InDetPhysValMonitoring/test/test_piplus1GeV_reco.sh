#!/bin/bash
# art-description: art job for InDetPhysValMonitoring, Single ele 10GeV
# art-type: grid
# art-input: user.keli:user.keli.mc16_13TeV.422047.ParticleGun_single_piplus_Pt1.recon.RDO.e7967_e5984_s3126_r11774_tid20255143_00
# art-input-nfiles: 10
# art-cores: 4
# art-memory: 4096
# art-include: master/Athena
# art-output: *.root
# art-output: *.xml 
# art-output: art_core_*
# art-output: dcube*

set -x

echo "ArtProcess: $ArtProcess"
lastref_dir=last_results
script="`basename \"$0\"`"
success_run=0

case $ArtProcess in
  "start")
    echo "Starting"
    echo "List of files = " ${ArtInFile}
    ;;
  "end")
    echo "Ending"
    if [ ${success_run} -eq 0 ]  ;then
      echo "download latest result"
      art.py download --user=artprod --dst="$lastref_dir" "$ArtPackage" "$ArtJobName"
      ls -la "$lastref_dir"
      echo "Merging physval.root"
      hadd  physval.root art_core_*/physval.ntuple.root
      echo "postprocess"
      postProcessIDPVMHistos physval.root

      dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/dcube/config/IDPVMPlots_R22.xml"
      dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/ReferenceHistograms/physval_piplus1GeV_reco_r22.root"
      echo "compare with R21"
      $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
	   -p -x dcube \
	   -c ${dcubeXml} \
	   -r ${dcubeRef} \
	   physval.root
      echo "art-result: $? plots"

      echo "compare with last build"
      $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
	   -p -x dcube_last \
	   -c ${dcubeXml} \
	   -r last_results/physval.root \
	   physval.root
      echo "art-result: $? plots"
    else
      echo "reco failed"
    fi
    ;;
  *)
    echo "Test $ArtProcess"
    mkdir "art_core_${ArtProcess}"
    cd "art_core_${ArtProcess}"
    IFS=',' read -r -a file <<< "${ArtInFile}"
    file=${file[${ArtProcess}]}
    x="../$file"
    echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
    unset  ATHENA_NUM_PROC
    unset  ATHENA_PROC_NUMBER

    Reco_tf.py \
      --inputRDOFile $x \
      --outputNTUP_PHYSVALFile physval.ntuple.root \
      --outputAODFile   physval.AOD.root \
      --steering        doRAWtoALL \
      --checkEventCount False \
      --ignoreErrors    True \
      --maxEvents       -1 \
      --skipEvents      0 \
      --valid           True \
      --validationFlags doInDet \
      --autoConfiguration everything \
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
    echo "art-result: $rec_tf_exit_code reco_${file}"
    if [ $rec_tf_exit_code -ne 0 ]  ;then
       success_run=$rec_tf_exit_code 
    fi
    ls -lR
    ;;
esac
