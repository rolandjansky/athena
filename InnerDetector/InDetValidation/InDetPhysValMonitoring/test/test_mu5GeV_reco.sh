#!/bin/bash
# art-description: art job for InDetPhysValMonitoring, Single muon 100GeV
# art-type: grid
# art-input: mc16_13TeV.422036.ParticleGun_single_mu_Pt100.recon.RDO.e7967_e5984_s3126_r11774_tid20254958_00
# art-input-nfiles: 10
# art-cores: 4
# art-include: master/Athena
# art-output: *.root
# art-output: *.xml 
# art-output: art_core_*
# art-output: dcube

set -x

echo "ArtProcess: $ArtProcess"

case $ArtProcess in
  "start")
    echo "Starting"
    echo "List of files = " ${ArtInFile}
    ;;
  "end")
    echo "Ending"
    echo "Merging physval.root"
    hadd  physval.root art_core_*/physval.ntuple.root
    echo "postprocess"
    postProcessIDPVMHistos physval.root

    dcubeName="Ele100"
    dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/dcube/config/IDPVMPlots_R22.xml"
    dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/ReferenceHistograms/physval_mu100GeV.root"

    bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName physval.root $dcubeXml $dcubeRef

    echo  "art-result: $? plot"
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
      --steering        doRAWtoALL \
      --checkEventCount False \
      --ignoreErrors    True \
      --maxEvents       -1 \
      --valid           True \
      --validationFlags doInDet \
      --preExec 'from InDetRecExample.InDetJobProperties import InDetFlags; \
      InDetFlags.doSlimming.set_Value_and_Lock(False); rec.doTrigger.set_Value_and_Lock(False); \
      from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; \
      InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True); \
      InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(False); \
      InDetPhysValFlags.doValidateGSFTracks.set_Value_and_Lock(False); \
      rec.doDumpProperties=True; rec.doCalo=False; rec.doEgamma=False; \
      rec.doForwardDet=False; rec.doInDet=True; rec.doJetMissingETTag=False; \
      rec.doLArg=False; rec.doLucid=False; rec.doMuon=False; rec.doMuonCombined=False; \
      rec.doSemiDetailedPerfMon=True; rec.doTau=False; rec.doTile=False;\
      from ParticleBuilderOptions.AODFlags import AODFlags;\
      AODFlags.ThinGeantTruth.set_Value_and_Lock(False);  \
      AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False); \
      AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False);\
      AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False) '
    echo  "art-result: $? reco_${ArtInFile}"
    ls -lR
    ;;
esac
