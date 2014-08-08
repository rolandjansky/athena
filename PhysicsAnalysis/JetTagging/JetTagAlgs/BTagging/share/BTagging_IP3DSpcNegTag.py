# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DSpcNegNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DSpcNegNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DSpcNegNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcNegNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DSpcNegTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DSpcNegTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DSpcNegTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcNegTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DSpcNeg = Analysis__SVForIPTool(name = "SVForIPTool_IP3DSpcNeg",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DSpcNeg
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DSpcNeg

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DSpcNegBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DSpcNegBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DSpcNegDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DSpcNegDetailedTrackGradeFactory",
                                                                          OutputLevel = BTaggingFlags.OutputLevel,
                                                                          hitBLayerGrade=True,
                                                                          TrackSummaryTool = None,
                                                                          useDetailSharedHitInfo = True,
                                                                          useSharedHitInfo = True)

ToolSvc += IP3DSpcNegBasicTrackGradeFactory
ToolSvc += IP3DSpcNegDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcNegBasicTrackGradeFactory
  print IP3DSpcNegDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DSpcNegTagTool = Analysis__IPTag(
                              name = "IP3DSpcNegTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DSpcNegNewLikelihood,
                              trackSelectorTool = IP3DSpcNegTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              usePosIP = True,
                              useNegIP = False,
                              flipIPSign = True,
                              SignWithSvx = True,
                              writeInfoPlus = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP3DSpcNeg,
#                              trackGradeFactory = IP3DSpcNegBasicTrackGradeFactory,
                              trackGradeFactory = IP3DSpcNegDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DSpcNegTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcNegTagTool
