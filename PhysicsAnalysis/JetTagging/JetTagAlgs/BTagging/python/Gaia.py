# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def get_gaia(jet_collection_list, 
             calibration_tool, 
             output_level=3, 
             runmodus='analysis', 
             output_threshold=3, 
             do_neg=False, 
             ): 
  from AthenaCommon.AppMgr import ToolSvc

  from JetTagTools.JetTagToolsConf import Analysis__GaiaVariablesFactory
  VariablesFactory = Analysis__GaiaVariablesFactory(
    name = "GaiaVariablesFactory",
    OutputLevel=output_level, 
    GaiaName = 'GaiaNeg' if do_neg else 'Gaia', 
    JetFitterCharmName = "JetFitterCharmNeg" if do_neg else "JetFitterCharm", 
    JetFitterName = "JetFitterTagNNFlip" if do_neg else "JetFitterTagNN", 
    )

  ToolSvc += VariablesFactory

  if output_level < output_threshold:
    print VariablesFactory

  from JetTagTools.JetTagToolsConf import Analysis__GaiaNNTool
  NNTool = Analysis__GaiaNNTool(
    name = "GaiaNNTool",
    OutputLevel=output_level, 
    # OutputLevel=output_level, # 2 = debug, 3 = info
    calibration_tool = calibration_tool, 
    calibration_directory = 'Gaia',
    calibration_subdirectory = 'NeuralNetwork'
  )

  ToolSvc += NNTool
  if output_level < output_threshold:
    print NNTool

  if do_neg: 
    supplemental_taggers = ['SV1Flip', 'SV0Flip']
    multiweight_taggers = ['IP3DNeg'] # add IPFordG
  else: 
    supplemental_taggers = ['SV1','SV0']
    multiweight_taggers = ['IP3D'] #,'IPFordG']

  ip3d_proxy = [1.0, 0.0, 0.0]
  proxy_likelihoods = {'IP3D':ip3d_proxy, 'IP3DNeg': ip3d_proxy}

  from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
  Gaia = Analysis__JetFitterTag( 
    name = "Gaia",
    OutputLevel = output_level, 
    Runmodus = runmodus, 
    jetfitterClassifier = NNTool,
    jetfitterVariablesFactory = VariablesFactory, 
    supplementalTaggers = supplemental_taggers, 
    multiweightSupplementalTaggers = multiweight_taggers, 
    useForcedCalibration = False,
    svxTaggerName = 'SVInfoPlus', 
    proxyLikelihoods = proxy_likelihoods, 
    storeOnlyBaseObject = True, 
    )

  ToolSvc += Gaia

  Gaia.jetCollectionList = jet_collection_list

  if output_level < output_threshold:
    print Gaia

  return Gaia
