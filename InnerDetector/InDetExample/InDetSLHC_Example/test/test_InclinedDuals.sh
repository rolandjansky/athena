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
dcube_sim_fixref="dcube_sim"
dcube_sim_lastref="dcube_sim_last"
dcube_rec_fixref="dcube"
dcube_rec_lastref="dcube_last"

artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
#artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="InclinedDuals"
script="`basename \"$0\"`"
evnt=$artdata/InDetSLHC_Example/inputs/EVNT.01485091._001049.pool.root.1
hits_ref=$artdata/InDetSLHC_Example/inputs/InclinedDuals_HITS.root
if [ $dosim -ne 0 ]; then
  hits=physval.HITS.root
else
  hits="$hits_ref"
fi
if [ $dorec -ne 0 ]; then
  esd=physval.ESD.root
  daod=physval.DAOD_IDTRKVALID.root
else
  esd=$artdata/InDetSLHC_Example/inputs/InclinedDuals_ESD.root
  daod=$artdata//InDetSLHC_Example/inputs/physval.DAOD_IDTRKVALID.root
fi
#jo=$artdata/InDetSLHC_Example/jobOptions/PhysValITk_jobOptions.py moved to share/
dcubemon_sim=SiHitValid.root
dcubemon_rec=physval.root
dcubecfg_sim=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_SiHitValid.xml
dcubecfg_rec=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_physval.xml
dcuberef_sim=$artdata/InDetSLHC_Example/ReferenceHistograms/InclinedDuals_SiHitValid.root
dcuberef_rec=$artdata/InDetSLHC_Example/ReferenceHistograms/InclinedDuals_physval.root
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube
lastref_dir=last_results

if [ \( $dosim -ne 0 -a -n "$dcube_sim_lastref" \) -o \( $dophy -ne 0 -a -n "$dcube_rec_lastref" \) ]; then
  run art.py download --user=artprod --dst="$lastref_dir" InDetSLHC_Example "$script"
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

  # Sim step: based on RTT SimInclinedDuals_GMX job:
  run Sim_tf.py \
    --inputEVNTFile   "$evnt" \
    --outputHITSFile  "$hits" \
    --skipEvents      0 \
    --maxEvents       10 \
    --randomSeed      873254 \
    --geometryVersion ATLAS-P2-ITK-20-03-00_VALIDATION \
    --conditionsTag   OFLCOND-MC15c-SDR-14-03 \
    --truthStrategy   MC15aPlus \
    --DataRunNumber   242000 \
    --preInclude  all:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py' \
    --preExec     all:'from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedDuals";from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags; XMLReaderFlags.readXMLfromDB.set_Value_and_Lock(False);' \
    --postInclude all:'PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/postInclude.SLHC_Setup.py,InDetSLHC_Example/postInclude.SiHitAnalysis.py' \
    --postExec    EVNTtoHITS:'ServiceMgr.DetDescrCnvSvc.DoInitNeighbours=False; from AthenaCommon import CfgGetter; CfgGetter.getService("ISF_MC15aPlusTruthService").BeamPipeTruthStrategies+=["ISF_MCTruthStrategyGroupIDHadInt_MC15"];'
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

  # Reco step: based on RecoInclinedDuals_GMX RTT job
  # some preExecs for E2D step are workarounds - should be revisited. 
  run Reco_tf.py \
    --inputHITSFile    "$hits" \
    --outputRDOFile    physval.RDO.root \
    --outputESDFile    "$esd" \
    --outputAODFile    physval.AOD.root \
    --outputDAOD_IDTRKVALIDFile "$daod" \
    --maxEvents        10 \
    --digiSteeringConf StandardInTimeOnlyTruth \
    --geometryVersion  ATLAS-P2-ITK-20-03-00 \
    --conditionsTag    OFLCOND-MC15c-SDR-14-03 \
    --DataRunNumber    242000 \
      --steering doRAWtoALL \
      --postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' \
      HITtoRDO:'InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' \
      RAWtoALL:'InDetSLHC_Example/postInclude.DigitalClustering.py' \
      --preExec     all:'from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedDuals";from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags; XMLReaderFlags.readXMLfromDB.set_Value_and_Lock(False);' \
      HITtoRDO:'from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' \
      RAWtoALL:'from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doStandardPlots.set_Value_and_Lock(True);from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr +=PixelCalibSvc();InDetFlags.useDCS.set_Value_and_Lock(True);ServiceMgr.PixelCalibSvc.DisableDB=True;from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags;InDetDxAODFlags.DumpLArCollisionTime.set_Value_and_Lock(False);InDetDxAODFlags.DumpSctInfo.set_Value_and_Lock(True);InDetDxAODFlags.ThinHitsOnTrack.set_Value_and_Lock(False)' \
      ESDtoDPD:'rec.DPDMakerScripts.set_Value_and_Lock(["PrimaryDPDMaker/PrimaryDPDMaker.py"]);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.useDCS.set_Value_and_Lock(True);from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr +=PixelCalibSvc();ServiceMgr.PixelCalibSvc.DisableDB=True' \
      --preInclude  all:'InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' \
      HITtoRDO:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py' \
      default:'InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
      RDOMergeAthenaMP:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py' \
      POOLMergeAthenaMPAOD0:'InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \
      POOLMergeAthenaMPDAODIDTRKVALID0:'InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \
      --postExec    HITtoRDO:'pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' \
     RAWtoALL:'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO;from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg;decorators = InDetPhysValDecoratorAlg();topSequence += decorators'  
  reco_stat=$?
  echo "art-result: $reco_stat reco"
  if [ "$reco_stat" -ne 0 ]; then
    echo "$script: Reco_tf.py isn't working yet. Remove jobReport.json to prevent pilot declaring a failed job."
    run rm -f jobReport.json
  fi

fi

if [ $dophy -ne 0 ]; then

  run ls -lLU "$esd"

  if [ ! -s "$esd" ]; then
    echo "$script: Reco_tf output '$esd' not created - exit" 2>&1
    echo "art-result: 21 physval"
#   test -n "$dcube_rec_fixref"  && echo "art-result: 22 plot-fixref"
    test -n "$dcube_rec_lastref" && echo "art-result: 22 plot"
    exit
  fi

  # Run InDetPhysValMonitoring on ESD.
  # It should eventually be possible to include this in the reco step, but this needs Reco_tf to support the ITk IDPVM setup.
  ( set -x
    inputDAOD_IDTRKVALIDFile="$daod" exec athena.py InDetSLHC_Example/PhysValITk_jobOptions.py
  )
  echo "art-result: $? physval"

  # DCube InDetPhysValMonitoring performance plots
  dcube InDetPhysValMonitoring plot "$dcubemon_rec" "$dcubecfg_rec" "$lastref_dir/$dcubemon_rec" "$dcube_rec_lastref"
  dcube InDetPhysValMonitoring ""   "$dcubemon_rec" "$dcubecfg_rec"              "$dcuberef_rec" "$dcube_rec_fixref"

fi
