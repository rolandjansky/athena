#!/bin/bash
# art-description: Upgrade release for Run2 - reco only
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube_sim
# art-output: dcube

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

dosim=0
dorec=1
artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
#artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="run2"
evnt=$artdata/InDetSLHC_Example/inputs/EVNT.12860049._002516.pool.root.1
hits_ref=$artdata/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3126/HITS.12860054._032508.pool.root.1
if [ $dosim -ne 0 ]; then
  hits=physval.HITS.root
else
  hits="$hits_ref"
fi
dcubemon_sim=SiHitValid.root
dcubemon_rec=physval.root
dcubecfg_sim=$artdata/InDetSLHC_Example/dcube/config/run2_SiHitValid.xml
dcuberef_sim=$artdata/InDetSLHC_Example/ReferenceHistograms/run2_SiHitValid.root
dcubecfg_rec=$artdata/InDetSLHC_Example/dcube/config/run2_physval.xml
dcuberef_rec=$artdata/InDetSLHC_Example/ReferenceHistograms/run2_physval.root
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube

if [ $dosim -ne 0 ]; then

  ls -lL "$evnt"

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
    --postInclude     default:'RecJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SiHitAnalysis.py'
  echo "art-result: $? sim"

  ls -lL "$dcubemon_sim" "$art_dcube" "$dcubecfg_sim" "$dcuberef_sim"

  if [ ! -s "$dcubemon_sim" ]; then
    echo "`basename \"$0\"`: Sim_tf output '$hits' not created. Skip Sim DCube step." 2>&1
    echo "art-result: 20 sim-plot"
  else
    # DCube Sim hit plots
    run "$art_dcube" "$name" "$dcubemon_sim" "$dcubecfg_sim" "$dcuberef_sim"
    echo  "art-result: $? sim-plot"
    run mv -f dcube dcube_sim
  fi

fi

if [ $dorec -ne 0 ]; then

  if [ $dosim -ne 0 ] && [ ! -s "$hits" ] && [ -s "$hits_ref" ]; then
    echo "`basename \"$0\"`: Sim_tf output '$hits' not created. Run Reco_tf on '$hits_ref' instead." 2>&1
    hits="$hits_ref"
  fi

  ls -lL "$hits"

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

  ls -lL "$dcubemon_rec" "$art_dcube" "$dcubecfg_rec" "$dcuberef_rec"

  if [ ! -s "$dcubemon_rec" ]; then
    echo "`basename \"$0\"`: InDetPhysValMonitoring output '$dcubemon_rec' not created - exit" 2>&1
    echo "art-result: 22 plot"
    exit
  fi

  # DCube InDetPhysValMonitoring performance plots
  run "$art_dcube" "$name" "$dcubemon_rec" "$dcubecfg_rec" "$dcuberef_rec"
  echo "art-result: $? plot"

fi
