# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP1DNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP1DNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
						 taggerName = "IP1D",
						 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP1DNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP1DNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP1DTrackSelector = Analysis__TrackSelector( 
                                         name = "IP1DTrackSelector",
                                         OutputLevel = BTaggingFlags.OutputLevel,
                                         trackToVertexTool = BTagTrackToVertexTool
                                        )
ToolSvc += IP1DTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP1DTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP1D = Analysis__SVForIPTool(name = "SVForIPTool_IP1D",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP1D
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP1D


from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP1DBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP1DBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

ToolSvc += IP1DBasicTrackGradeFactory
if BTaggingFlags.OutputLevel < 3:
  print IP1DBasicTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP1DTagTool = Analysis__IPTag(
                              name = "IP1DTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "1D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP1DNewLikelihood,
                              trackSelectorTool = IP1DTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
                              trackGradePartitions = [ "Good", "Shared" ],
                              RejectBadTracks = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP1D,
                              trackGradeFactory = IP1DBasicTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,	
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP1DTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP1DTagTool
