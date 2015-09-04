# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def get_jet_fitter_vx_finder(
  vx_finder_name, 
  init_helper, 
  routines, 
  utils, 
  extrapolator, 
  track_selector, 
  mode3dfinder, 
  seq_vx_fitter, 
  vxp_container, 
  b_tag_tool, 
  output_level = 3, 
  cut_factor = None, 
  output_threshold = 3): 
  
  """builds a replacement for NewJetFitterVxFinder"""
  
  from AthenaCommon.AppMgr import ToolSvc

  # --- looser cuts
  loose_cuts = dict(
    cutCompatibilityPrimaryVertexForPositiveLifetimeTracks = 0.2, 
    cutCompatibilityPrimaryVertexForNegativeLifetimeTracks = 0.1, 
    VertexClusteringProbabilityCut = 0.01, 
    twoVertexProbabilityCut = 0.05, 
    cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection = 0.2, 
    cutCompatibilityPrimaryVertexBothTracksForBFirstSelection = 0.05, 
    cutPtBothTracksForBFirstSelection = 400.0, 
    cutPtSingleTrackForBSecondSelection = 500.0, 
    )

  # --- looserer cuts
  if cut_factor: 
    for cut in loose_cuts.keys(): 
      if 'Pt' in cut: 
        continue 
      loose_cuts[cut] *= cut_factor
  else: 
    loose_cuts = dict(
      VertexClusteringProbabilityCut = 0.005, 
      )

  from InDetSecVxFinderTool.InDetSecVxFinderToolConf import (
    InDet__InDetImprovedJetFitterVxFinder ) 

  VxFinder = InDet__InDetImprovedJetFitterVxFinder( 
    name = vx_finder_name, 
    JetFitterInitializationHelper = init_helper,
    JetFitterRoutines = routines,
    InDetJetFitterUtils = utils,
    Extrapolator = extrapolator,
    TrackSelector = track_selector,
    Mode3dFinder = mode3dfinder,
    SequentialVertexFitter = seq_vx_fitter,
    # VxPrimaryContainer="VxPrimaryCandidateAOD",
    VxPrimaryContainer = vxp_container,
    OutputLevel = output_level,
    MaxNumDeleteIterations = 30,
    VertexProbCut = 0.001,
    MaxClusteringIterations = 30,
    **loose_cuts
    )

  ToolSvc += VxFinder
  if output_level < output_threshold:
    print VxFinder
        
  b_tag_tool.SecVtxFinderList+= [ VxFinder ]
  b_tag_tool.SecVtxFinderTrackNameList+=["Tracks"]

  return VxFinder
                             

def get_jet_fitter_charm(jet_collection_list, 
                         calibration_tool, 
                         output_level=3, 
                         runmodus='analysis', 
                         name_extension='Charm', 
                         vx_finder_name='NewJetFitterVxFinder', 
                         output_threshold=3, 
                         do_neg=False, 
                         ): 
  from AthenaCommon.AppMgr import ToolSvc

  from JetTagTools.JetTagToolsConf import Analysis__JetFitterCharmVariablesFactory
  VariablesFactory = Analysis__JetFitterCharmVariablesFactory( 
    name = "JetFitterVariablesFactory" + name_extension,
    OutputLevel=output_level, 
    JetFitterInstance = "JetFitter" + name_extension,
    secVxFinderName = vx_finder_name, 
    usePtCorrectedMass=True, 
    revertFromPositiveToNegativeTags=do_neg)	

  ToolSvc += VariablesFactory

  if output_level < output_threshold:
    print VariablesFactory

  from JetTagTools.JetTagToolsConf import Analysis__JetFitterCharmNNTool
  JetFitterCharmNNTool = Analysis__JetFitterCharmNNTool( 
    name = "JetFitter" + name_extension + "NNTool",
    calibration_tool = calibration_tool, 
    calibration_directory = 'JetFitterCharm',
    calibration_subdirectory = 'NeuralNetwork'
  )

  ToolSvc += JetFitterCharmNNTool
  if output_level < output_threshold:
    print JetFitterCharmNNTool

  if do_neg: 
    supplemental_taggers = ['IP3DNeg','SV1Flip']
  else: 
    supplemental_taggers = ['IP3D','SV1']

  from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
  JetFitterCharm = Analysis__JetFitterTag( 
    name = "JetFitter" + name_extension,
    OutputLevel = output_level, 
    Runmodus = runmodus, 
    jetfitterClassifier = JetFitterCharmNNTool,
    jetfitterVariablesFactory = VariablesFactory, 
    supplementalTaggers = supplemental_taggers, 
    useForcedCalibration = False,
    svxTaggerName = 'SVInfoPlus', 
    )

  ToolSvc += JetFitterCharm

  JetFitterCharm.jetCollectionList = jet_collection_list

  if output_level < output_threshold:
    print JetFitterCharm

  return JetFitterCharm


