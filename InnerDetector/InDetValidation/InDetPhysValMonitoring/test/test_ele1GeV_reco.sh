#!/bin/bash
# art-description: Standard test for Run2 with 1 GeV muon input
# art-type: grid
# art-input: mc16_13TeV.422028.ParticleGun_single_ele_Pt5.recon.RDO.e7967_e5984_s3126_r11774_tid20254991_00
# art-include: master/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*

echo "List of files = " ${ArtInFile}

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

# Following specify DCube output directories. Set empty to disable.
dcube_rec_fixref="dcube"
#dcube_rec_lastref="dcube_last"

artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art

name="run2"
script="`basename \"$0\"`"
dcubemon_rec=physval.root
dcubecfg_rec=$artdata/InDetPhysValMonitoring/dcube/config/IDPVMPlots_R22.xml
dcuberef_rec=$artdata/InDetPhysValMonitoring/ReferenceHistograms/physval_ele5GeV.root
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube

#lastref_dir=last_results

dcube() {
  # Run DCube and print art-result (if $2 is not empty)
  step="$1" statname="$2" dcubemon="$3" dcubecfg="$4" dcuberef="$5" dcubedir="$6"
  test -n "$dcubedir" || return
  run ls -lLU "$art_dcube" "$dcubemon" "$dcubecfg" "$dcuberef"
  if [ ! -s "$dcubemon" ]; then
    echo "$script: $step output '$dcubemon' not created. Don't create $dcubedir output." 2>&1
    test -n "$statname" && echo "art-result: 20 $statname"
    return
  fi
  if [ ! -s "$dcuberef" ]; then
    echo "$script: $step DCube reference '$dcuberef' not available. Don't create $dcubedir output." 2>&1
    test -n "$statname" && echo "art-result: 21 $statname"
    return
  fi
  echo "$script: DCube comparison of $step output '$dcubemon' with '$dcuberef' reference"
  keep=""
  test "$dcubedir" != "dcube" -a -d "dcube" && keep="dcube_keep_`uuidgen`"
  test -n "$keep" && run mv -f dcube "$keep"
  run "$art_dcube" "$name" "$dcubemon" "$dcubecfg" "$dcuberef"
  dcube_stat=$?
  test -n "$statname" && echo "art-result: $dcube_stat $statname"
  test "$dcubedir" != "dcube" && run mv -f dcube "$dcubedir"
  test -n "$keep" && run mv -f "$keep" dcube
}


# Reco step based on test InDetPhysValMonitoring ART setup from Josh Moss.
run Reco_tf.py \
  --inputRDOFile ${ArtInFile} \
  --outputAODFile   physval.AOD.root \
  --outputNTUP_PHYSVALFile "$dcubemon_rec" \
  --steering        doRAWtoALL \
  --checkEventCount False \
  --ignoreErrors    True \
  --maxEvents       50000 \
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
echo "art-result: $? reco"
#if [ rec_tf_exit_code  eq 0 ]  ;then
#  run art.py download --user=artprod --dst="$lastref_dir" InDetPhysValMonitoring "$script"
#  run ls -la "$lastref_dir"
#fi


# DCube InDetPhysValMonitoring performance plots
#dcube InDetPhysValMonitoring plot "$dcubemon_rec" "$dcubecfg_rec" "$lastref_dir/$dcubemon_rec" "$dcube_rec_lastref"
dcube InDetPhysValMonitoring ""   "$dcubemon_rec" "$dcubecfg_rec"              "$dcuberef_rec" "$dcube_rec_fixref"
