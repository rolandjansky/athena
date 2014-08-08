# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool

# Likelihood tool
SoftElelectronTagNewLikelihood = Analysis__NewLikelihoodTool( 
    name = "SoftElectronTagNewLikelihoodTool",
    OutputLevel = BTaggingFlags.OutputLevel,
    taggerName = "SoftEl",
    calibrationTool = BTagCalibrationBrokerTool
    )
ToolSvc += SoftElelectronTagNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print SoftElelectronTagNewLikelihood

# Track selector tool
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
SoftElectronTagTrackSelector = Analysis__TrackSelector( 
    name = "SoftElectronTagTrackSelector",
    trackToVertexTool = BTagTrackToVertexTool,
    OutputLevel = BTaggingFlags.OutputLevel
    )
ToolSvc += SoftElectronTagTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print SoftElectronTagTrackSelector

#
from JetTagTools.JetTagToolsConf import Analysis__SoftElectronTag
SoftElectronTagTool = Analysis__SoftElectronTag(
    name = "SoftElectronTag",
    Runmodus = BTaggingFlags.Runmodus,
    OutputLevel = BTaggingFlags.OutputLevel,
    originalElCollectionName = "ElectronAODCollection",
    writeInfoPlus     = True, 
    LikelihoodTool    = SoftElelectronTagNewLikelihood,
    trackSelectorTool = SoftElectronTagTrackSelector,
    trackToVertexTool = BTagTrackToVertexTool,
    TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
    jetCollectionList = BTaggingFlags.Jets,
    calibrationTool = BTagCalibrationBrokerTool
    )
ToolSvc += SoftElectronTagTool
if BTaggingFlags.OutputLevel < 3:
  print SoftElectronTagTool
