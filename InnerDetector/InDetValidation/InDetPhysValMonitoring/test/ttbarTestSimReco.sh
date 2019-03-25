#!/bin/bash
# art-description: Standard test for Run2 with ttbar input
# art-type: grid
# art-include: 21.0/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

# Following specify which steps to run.
dosim=1
dorec=1

# Following specify DCube output directories. Set empty to disable.
dcube_sim_fixref="dcube_sim"
dcube_sim_lastref="dcube_sim_last"
dcube_rec_fixref="dcube"
dcube_rec_lastref="dcube_last"

artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
#artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="run2"
script="`basename \"$0\"`"
evnt=$artdata/InDetPhysValMonitoring/inputs/EVNT.12860049._002516.pool.root.1
hits_ref=$artdata/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3126/HITS.12860054._032508.pool.root.1
if [ $dosim -ne 0 ]; then
  hits=physval.HITS.root
else
  hits="$hits_ref"
fi
dcubemon_sim=SiHitValid.root
dcubemon_rec=physval.root
dcubecfg_sim=$artdata/InDetPhysValMonitoring/dcube/config/run2_SiHitValid.xml
dcuberef_sim=$artdata/InDetPhysValMonitoring/ReferenceHistograms/run2_SiHitValid.root
dcubecfg_rec=$artdata/InDetPhysValMonitoring/dcube/config/run2_physval.xml
dcuberef_rec=$artdata/InDetPhysValMonitoring/ReferenceHistograms/run2_physval.root
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube

lastref_dir=last_results

if [ \( $dosim -ne 0 -a -n "$dcube_sim_lastref" \) -o \( $dorec -ne 0 -a -n "$dcube_rec_lastref" \) ]; then
  run art.py download --user=artprod --dst="$lastref_dir" InDetPhysValMonitoring "$script"
  run ls -la "$lastref_dir"
fi

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

if [ $dosim -ne 0 ]; then

  run ls -lL "$evnt"

  # Sim step (based on PanDA job 3777178576 which made HITS.12860054._032508.pool.root.1, which is the input for AMI config q221):
  run Sim_tf.py \
    --AMITag          s3126 \
    --inputEVNTFile   "$evnt" \
    --outputHITSFile  "$hits" \
    --skipEvents      3000 \
    --maxEvents       10 \
    --runNumber       410470 \
    --firstEvent      24303001 \
    --randomSeed      24304 \
    --DataRunNumber   284500 \
    --physicsList     FTFP_BERT_ATL_VALIDATION \
    --truthStrategy   MC15aPlus \
    --simulator       FullG4 \
    --DBRelease       all:'current' \
    --conditionsTag   default:'OFLCOND-MC16-SDR-14' \
    --geometryVersion default:'ATLAS-R2-2016-01-00-01_VALIDATION' \
    --preExec         EVNTtoHITS:'simFlags.SimBarcodeOffset.set_Value_and_Lock(200000)' \
                      EVNTtoHITS:'simFlags.TRTRangeCut=30.0; simFlags.TightMuonStepping=True' \
    --preInclude      EVNTtoHITS:'SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
    --postInclude     default:'RecJobTransforms/UseFrontier.py,InDetPhysValMonitoring/postInclude.SiHitAnalysis.py'
  echo "art-result: $? sim"

  # DCube Sim hit plots
  dcube Sim_tf sim-plot "$dcubemon_sim" "$dcubecfg_sim" "$lastref_dir/$dcubemon_sim" "$dcube_sim_lastref"
  dcube Sim_tf ""       "$dcubemon_sim" "$dcubecfg_sim"              "$dcuberef_sim" "$dcube_sim_fixref"

fi

if [ $dorec -ne 0 ]; then

  if [ $dosim -ne 0 ] && [ ! -s "$hits" ] && [ -s "$hits_ref" ]; then
    echo "$script: Sim_tf output '$hits' not created. Run Reco_tf on '$hits_ref' instead." 2>&1
    hits="$hits_ref"
  fi

  run ls -lL "$hits"

  # Reco step based on test InDetPhysValMonitoring ART setup from Josh Moss.
  run Reco_tf.py \
    --inputHITSFile   "$hits" \
    --outputAODFile   physval.AOD.root \
    --outputNTUP_PHYSVALFile "$dcubemon_rec" \
    --steering        doRAWtoALL \
    --checkEventCount False \
    --ignoreErrors    True \
    --maxEvents       10 \
    --valid           True \
    --validationFlags doInDet \
    --preExec 'from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doSlimming.set_Value_and_Lock(False); rec.doTrigger.set_Value_and_Lock(False); from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True); InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(True); InDetPhysValFlags.doValidateGSFTracks.set_Value_and_Lock(False); rec.doDumpProperties=True; rec.doCalo=False; rec.doEgamma=False; rec.doForwardDet=False; rec.doInDet=True; rec.doJetMissingETTag=False; rec.doLArg=False; rec.doLucid=False; rec.doMuon=False; rec.doMuonCombined=False; rec.doSemiDetailedPerfMon=True; rec.doTau=False; rec.doTile=False;'
  echo "art-result: $? reco"

  # DCube InDetPhysValMonitoring performance plots
  dcube InDetPhysValMonitoring plot "$dcubemon_rec" "$dcubecfg_rec" "$lastref_dir/$dcubemon_rec" "$dcube_rec_lastref"
  dcube InDetPhysValMonitoring ""   "$dcubemon_rec" "$dcubecfg_rec"              "$dcuberef_rec" "$dcube_rec_fixref"

fi

