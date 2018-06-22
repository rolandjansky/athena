#!/bin/bash
# art-description: InclinedDuals sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube_sim
# art-output: dcube

# Fix ordering of output in logfile
exec 2>&1
run() { (set -x; exec "$@") }

dosim=1
dorec=0    # Reco_tf.py not yet working
dophy=0    # Set dophy=1 to run InDetPhysValMonitoring over old ESD
artdata=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art
#artdata=/eos/atlas/atlascerngroupdisk/data-art/grid-input

name="InclinedDuals"
evnt=$artdata/InDetSLHC_Example/inputs/EVNT.01485091._001049.pool.root.1
hits_ref=$artdata/InDetSLHC_Example/inputs/InclinedDuals_HITS.root
if [ $dosim -ne 0 ]; then
  hits=physval.HITS.root
else
  hits="$hits_ref"
fi
if [ $dorec -ne 0 ]; then
  esd=physval.ESD.root
else
  esd=$artdata/InDetSLHC_Example/inputs/InclinedDuals_ESD.root
fi
jo=$artdata/InDetSLHC_Example/jobOptions/PhysValITk_jobOptions.py
dcubemon_sim=SiHitValid.root
dcubemon_rec=physval.root
dcubecfg_sim=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_SiHitValid.xml
dcubecfg_rec=$artdata/InDetSLHC_Example/dcube/config/InclinedDuals_physval.xml
dcuberef_sim=$artdata/InDetSLHC_Example/ReferenceHistograms/InclinedDuals_SiHitValid.root
dcuberef_rec=$artdata/InDetSLHC_Example/ReferenceHistograms/InclinedDuals_physval.root
art_dcube=/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube

#ls -lL "$evnt" "$hits" "$hits_ref" "$esd" "$jo" "$art_dcube" "$dcubecfg_sim" "$dcuberef_sim" "$dcubecfg_rec" "$dcuberef_rec"

if [ $dosim -ne 0 ]; then

  ls -lL "$evnt"

  # Sim step: based on RTT SimInclinedDuals_GMX job:
  run Sim_tf.py \
    --inputEVNTFile   "$evnt" \
    --outputHITSFile  "$hits" \
    --skipEvents      0 \
    --maxEvents       10 \
    --randomSeed      873254 \
    --geometryVersion ATLAS-P2-ITK-20-00-00_VALIDATION \
    --conditionsTag   OFLCOND-MC15c-SDR-14-03 \
    --truthStrategy   MC15aPlus \
    --DataRunNumber   242000 \
    --preInclude  all:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py' \
    --preExec     all:'from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedDuals";' \
    --postInclude all:'PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/postInclude.SLHC_Setup.py,InDetSLHC_Example/postInclude.SiHitAnalysis.py' \
    --postExec    EVNTtoHITS:'ServiceMgr.DetDescrCnvSvc.DoInitNeighbours=False; from AthenaCommon import CfgGetter; CfgGetter.getService("ISF_MC15aPlusTruthService").BeamPipeTruthStrategies+=["ISF_MCTruthStrategyGroupIDHadInt_MC15"];'
  echo "art-result: $? sim"

  ls -lL "$dcubemon_sim" "$art_dcube" "$dcubecfg_sim" "$dcuberef_sim"

  if [ ! -s "$dcubemon_sim" ]; then
    echo "`basename \"$0\"`: Sim_tf output '$hits' not created. Skip Sim DCube step." 2>&1
    echo "art-result: 20 sim-plot"
  else
    # DCube Sim hit plots
    run "$art_dcube" "$name" "$dcubemon_sim" "$dcubecfg_sim" "$dcuberef_sim"
    echo "art-result: $? sim-plot"
    run mv -f dcube dcube_sim
  fi

fi

if [ $dorec -ne 0 ]; then

  if [ $dosim -ne 0 ] && [ ! -s "$hits" ] && [ -s "$hits_ref" ]; then
    echo "`basename \"$0\"`: Sim_tf output '$hits' not created. Run Reco_tf on '$hits_ref' instead." 2>&1
    hits="$hits_ref"
  fi

  ls -lL "$hits"

  # Reco step: based on RecoInclinedDuals_GMX RTT job
  run Reco_tf.py \
    --inputHITSFile    "$hits" \
    --outputRDOFile    physval.RDO.root \
    --outputESDFile    "$esd" \
    --outputAODFile    physval.AOD.root \
    --outputDAOD_IDTRKVALIDFile physval.DAOD_IDTRKVALID.root \
    --maxEvents        10 \
    --digiSteeringConf StandardInTimeOnlyTruth \
    --geometryVersion  ATLAS-P2-ITK-20-00-00 \
    --conditionsTag    OFLCOND-MC15c-SDR-14-03 \
    --DataRunNumber    242000 \
    --postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' \
                  HITtoRDO:'InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' \
                  RAWtoESD:'InDetSLHC_Example/postInclude.DigitalClustering.py,InDetSLHC_Example/postInclude.NoJetPtFilter.py' \
    --preExec     all:'from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True); SLHC_Flags.LayoutOption="InclinedDuals";' \
                  HITtoRDO:'from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' \
                  RAWtoESD:'from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doStandardPlots.set_Value_and_Lock(True)' \
                  ESDtoDPD:'rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \
    --preInclude  all:'InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' \
                  HITtoRDO:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py' \
                  default:'InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' \
                  RDOMergeAthenaMP:'InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py' \
                  POOLMergeAthenaMPAOD0:'InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \
                  POOLMergeAthenaMPDAODIDTRKVALID0:'InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \
    --postExec    HITtoRDO:'pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' \
                  RAWtoESD:'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO;'
  echo "art-result: $? reco"

fi

if [ $dophy -ne 0 ]; then

  ls -lL "$esd" "$jo"

  if [ ! -s "$esd" ]; then
    echo "`basename \"$0\"`: Reco_tf output '$esd' not created - exit" 2>&1
    echo "art-result: 21 physval"
    echo "art-result: 21 plot"
    exit
  fi

  # Run InDetPhysValMonitoring on ESD.
  # It should eventually be possible to include this in the reco step, but this needs Reco_tf to support the ITk IDPVM setup.
  ( set -x
    inputESDFile="$esd" athena.py "$jo"
  )
  echo "art-result: $? physval"

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
