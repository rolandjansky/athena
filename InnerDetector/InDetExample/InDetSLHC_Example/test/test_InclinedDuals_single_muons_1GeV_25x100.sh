#!/bin/bash
# art-description: InclinedDuals sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

# Following specify which steps to run.
dosim=1
dorec=1    # Reco_tf.py not yet working
dophy=1    # If dorec=0, set dophy=1 to run InDetPhysValMonitoring over old ESD

# Following specify DCube output directories. Set empty to disable.
dcube_sim_muons_1GeV_fixref="dcube_sim_muons_1GeV"
dcube_sim_muons_1GeV_lastref="dcube_sim_muons_1GeV_last"
dcube_digi_pixel_muons_1GeV_fixref="dcube_digi_pixel_muons_1GeV"
dcube_digi_pixel_muons_1GeV_lastref="dcube_digi_pixel_muons_1GeV_last"
dcube_digi_strip_muons_1GeV_fixref="dcube_digi_strip_muons_1GeV"
dcube_digi_strip_muons_1GeV_lastref="dcube_digi_strip_muons_1GeV_last"
dcube_rec_muons_1GeV_fixref="dcube_muons_1GeV"
dcube_rec_muons_1GeV_lastref="dcube_muons_1GeV_last"

#artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="InclinedDuals"
script="`basename \"$0\"`"

evnt_muons_1GeV=$artdata/InDetSLHC_Example/inputs/EVNT.09244569.*.pool.root.1
hits_ref_muons_1GeV=$artdata/InDetSLHC_Example/inputs/InclinedDuals_HITS_mu_1GeV.root

if [ $dosim -ne 0 ]; then
  hits_muons_1GeV=physval_muons_1GeV.HITS.root
else
  hits_muons_1GeV="$hits_ref_muons_1GeV"
fi
if [ $dorec -ne 0 ]; then
  esd_muons_1GeV=physval_muons_1GeV.ESD.root
  daod_muons_1GeV=physval_muons_1GeV.DAOD_IDTRKVALID.root
else
  esd_muons_1GeV=$artdata/InDetSLHC_Example/inputs/physval_muons_1GeV_25x100_2020.ESD.root
  daod_muons_1GeV=$artdata/InDetSLHC_Example/inputs/physval_muons_1GeV_25x100_2020.DAOD_IDTRKVALID.root
fi
#jo=$artdata/InDetSLHC_Example/jobOptions/PhysValITk_jobOptions.py moved to share/
dcubemon_muons_1GeV_sim=SiHitValid_muons_1GeV.root
dcubemon_muons_1GeV_digi_pixel=PixelRDOAnalysis_muons_1GeV.root
dcubemon_muons_1GeV_digi_strip=SCT_RDOAnalysis_muons_1GeV.root
dcubemon_muons_1GeV_rec=physval_muons_1GeV.root
dcubecfg_sim=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_SiHitValid_25x100.xml 
dcubecfg_digi_pixel=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_PixelRDOAnalysis_25x100.xml 
dcubecfg_digi_strip=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_SCT_RDOAnalysis_25x100.xml 
dcubecfg_rec=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_physval_25x100.xml
dcuberef_muons_1GeV_sim=$artdata/InDetSLHC_Example/ReferenceHistograms/InclinedDuals_muons_1GeV_SiHitValid_25x100_2020.root
dcuberef_muons_1GeV_rec=$artdata/InDetSLHC_Example/ReferenceHistograms/InclinedDuals_muons_1GeV_physval_25x100_2020.root
dcuberef_muons_1GeV_digi_pixel=$artdata/InDetSLHC_Example/ReferenceHistograms/PixelRDOAnalysis_muons_1GeV_25x100_2020.root
dcuberef_muons_1GeV_digi_strip=$artdata/InDetSLHC_Example/ReferenceHistograms/SCT_RDOAnalysis_muons_1GeV_25x100_2020.root
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube
lastref_muons_1GeV_dir=last_results_muons_1GeV

if [ \( $dosim -ne 0 -a -n "$dcube_sim_muons_1GeV_lastref" \) -o \( $dophy -ne 0 -a -n "$dcube_rec_muons_1GeV_lastref" \) ]; then
  run art.py download --user=artprod --dst="$lastref_muons_1GeV_dir" InDetSLHC_Example "$script"
  run ls -la "$lastref_muons_1GeV_dir"
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

  ## this runs single muons simulation
  run ls -lL "$evnt_muons_1GeV"

  # Sim step: based on RTT SimInclinedDuals_GMX job:
  run Sim_tf.py \
    --inputEVNTFile   "$evnt_muons_1GeV" \
    --outputHITSFile  "$hits_muons_1GeV" \
    --skipEvents      0 \
    --maxEvents       10000 \
    --randomSeed      873254 \
    --geometryVersion ATLAS-P2-ITK-20-00-00_VALIDATION \
    --conditionsTag   OFLCOND-MC15c-SDR-14-03 \
    --truthStrategy   MC15aPlus \
    --DataRunNumber   242000 \
    --preInclude  all:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py' \
    --preExec     all:'from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedDuals";' \
    --postInclude all:'PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/postInclude.SLHC_Setup.py,InDetSLHC_Example/postInclude.SiHitAnalysis.py' \
    --postExec    EVNTtoHITS:'ServiceMgr.DetDescrCnvSvc.DoInitNeighbours=False; from AthenaCommon import CfgGetter; CfgGetter.getService("ISF_MC15aPlusTruthService").BeamPipeTruthStrategies+=["ISF_MCTruthStrategyGroupIDHadInt_MC15"];'
  echo "art-result: $? sim"
  
  mv ./SiHitValid.root ./$dcubemon_muons_1GeV_sim

  # DCube Sim hit plots
  dcube Sim_tf sim-plot "$dcubemon_muons_1GeV_sim" "$dcubecfg_sim" "$lastref_muons_1GeV_dir/$dcubemon_muons_1GeV_sim" "$dcube_sim_muons_1GeV_lastref"
  dcube Sim_tf ""       "$dcubemon_muons_1GeV_sim" "$dcubecfg_sim"                         "$dcuberef_muons_1GeV_sim" "$dcube_sim_muons_1GeV_fixref"
  
fi

if [ $dorec -ne 0 ]; then

  ## Starting reconstruction for single muons
  if [ $dosim -ne 0 ] && [ ! -s "$hits_muons_1GeV" ] && [ -s "$hits_ref_muons_1GeV" ]; then
    echo "$script: Sim_tf output '$hits_muons_1GeV' not created. Run Reco_tf on '$hits_ref_muons_1GeV' instead." 2>&1
    hits_muons_1GeV="$hits_ref_muons_1GeV"
  fi

  run ls -lL "$hits_muons_1GeV"

  # Reco step: based on RecoInclinedDuals_GMX RTT job
  # some preExecs for E2D step are workarounds - should be revisited. 
  run Reco_tf.py \
    --inputHITSFile    "$hits_muons_1GeV" \
    --outputRDOFile    physval_muons_1GeV.RDO.root \
    --outputESDFile    "$esd_muons_1GeV" \
    --outputAODFile    physval_muons_10GeV.AOD.root \
    --outputDAOD_IDTRKVALIDFile "$daod_muons_1GeV" \
    --maxEvents        -1 \
    --digiSteeringConf StandardInTimeOnlyTruth \
    --geometryVersion  ATLAS-P2-ITK-20-00-00 \
    --conditionsTag    OFLCOND-MC15c-SDR-14-03 \
    --DataRunNumber    242000 \
    --steering doRAWtoALL \
    --postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4_25x100.py' \
                  HITtoRDO:'InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py,InDetSLHC_Example/postInclude.RDOAnalysis.py' \
                  RAWtoALL:'InDetSLHC_Example/postInclude.DigitalClustering.py' \
    --preExec     all:'from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedDuals";' \
                  HITtoRDO:'from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.doBichselSimulation.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' \
                  RAWtoALL:'from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doStandardPlots.set_Value_and_Lock(True);from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr +=PixelCalibSvc();InDetFlags.useDCS.set_Value_and_Lock(True);ServiceMgr.PixelCalibSvc.DisableDB=True;from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags;InDetDxAODFlags.DumpLArCollisionTime.set_Value_and_Lock(False);InDetDxAODFlags.DumpSctInfo.set_Value_and_Lock(True);InDetDxAODFlags.ThinHitsOnTrack.set_Value_and_Lock(False)' \
                  ESDtoDPD:'rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.useDCS.set_Value_and_Lock(True);from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr +=PixelCalibSvc();ServiceMgr.PixelCalibSvc.DisableDB=True' \
    --preInclude  all:'InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4_25x100.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' \
                  HITtoRDO:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' \
                  default:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
                  RDOMergeAthenaMP:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' \
                  POOLMergeAthenaMPAOD0:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \
                  POOLMergeAthenaMPDAODIDTRKVALID0:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \
    --postExec    HITtoRDO:'pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' \
                  RAWtoALL:'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO;from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg;decorators = InDetPhysValDecoratorAlg();topSequence += decorators'
  reco_stat=$?
  echo "art-result: $reco_stat reco"
  if [ "$reco_stat" -ne 0 ]; then
    echo "$script: Reco_tf.py isn't working yet. Remove jobReport.json to prevent pilot declaring a failed job."
    run rm -f jobReport.json
  fi
  
  mv ./PixelRDOAnalysis.root ./$dcubemon_muons_1GeV_digi_pixel
  mv ./SCT_RDOAnalysis.root ./$dcubemon_muons_1GeV_digi_strip
  
  # DCube digi plots
  dcube Reco_tf digi-plot "$dcubemon_muons_1GeV_digi_pixel" "$dcubecfg_digi_pixel" "$lastref_muons_1GeV_dir/$dcubemon_muons_1GeV_digi_pixel" "$dcube_digi_pixel_muons_1GeV_lastref"
  dcube Reco_tf  ""       "$dcubemon_muons_1GeV_digi_pixel" "$dcubecfg_digi_pixel"                           "$dcuberef_muons_1GeV_digi_pixel" "$dcube_digi_pixel_muons_1GeV_fixref"
  
  dcube Reco_tf digi-plot "$dcubemon_muons_1GeV_digi_strip" "$dcubecfg_digi_strip" "$lastref_muons_1GeV_dir/$dcubemon_muons_1GeV_digi_strip" "$dcube_digi_strip_muons_1GeV_lastref"
  dcube Reco_tf  ""       "$dcubemon_muons_1GeV_digi_strip" "$dcubecfg_digi_strip"                           "$dcuberef_muons_1GeV_digi_strip" "$dcube_digi_strip_muons_1GeV_fixref"
    
fi

if [ $dophy -ne 0 ]; then

  ## phys validation and dcube for single muons
  run ls -lLU "$esd_muons_1GeV"

  if [ ! -s "$esd_muons_1GeV" ]; then
    echo "$script: Reco_tf output '$esd_muons_1GeV' not created - exit" 2>&1
    echo "art-result: 21 physval"
#   test -n "$dcube_rec_fixref"  && echo "art-result: 22 plot-fixref"
    test -n "$dcube_rec_muons_1GeV_lastref" && echo "art-result: 22 plot"
    exit
  fi
  
  # Run InDetPhysValMonitoring on ESD.
  # It should eventually be possible to include this in the reco step, but this needs Reco_tf to support the ITk IDPVM setup.
  ( set -x
    inputDAOD_IDTRKVALIDFile="$daod_muons_1GeV" exec athena.py InDetSLHC_Example/PhysValITk_jobOptions.py
  )
  echo "art-result: $? physval"
  
  mv ./physval.root ./$dcubemon_muons_1GeV_rec
  
  # DCube InDetPhysValMonitoring performance plots
  dcube InDetPhysValMonitoring plot "$dcubemon_muons_1GeV_rec" "$dcubecfg_rec" "$lastref_muons_1GeV_dir/$dcubemon_muons_1GeV_rec" "$dcube_rec_muons_1GeV_lastref"
  dcube InDetPhysValMonitoring ""   "$dcubemon_muons_1GeV_rec" "$dcubecfg_rec"                         "$dcuberef_muons_1GeV_rec" "$dcube_rec_muons_1GeV_fixref"
  
fi
