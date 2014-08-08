# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DSpcNegNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DSpcNegNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DSpcNegNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcNegNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DSpcNegTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DSpcNegTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DSpcNegTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcNegTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DSpcNeg = Analysis__SVForIPTool(name = "SVForIPTool_IP2DSpcNeg",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DSpcNeg
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DSpcNeg

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DSpcNegBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DSpcNegBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DSpcNegDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DSpcNegDetailedTrackGradeFactory",
                                                                          OutputLevel = BTaggingFlags.OutputLevel,
                                                                          hitBLayerGrade=True,
                                                                          TrackSummaryTool = None,
                                                                          useDetailSharedHitInfo = True,
                                                                          useSharedHitInfo = True)

ToolSvc += IP2DSpcNegBasicTrackGradeFactory
ToolSvc += IP2DSpcNegDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcNegBasicTrackGradeFactory
  print IP2DSpcNegDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DSpcNegTagTool = Analysis__IPTag(
                              name = "IP2DSpcNegTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DSpcNegNewLikelihood,
                              trackSelectorTool = IP2DSpcNegTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              usePosIP = True,
                              useNegIP = False,
                              flipIPSign = True,
                              writeInfoPlus = False,
                              SignWithSvx = True,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DSpcNeg,
#                              trackGradeFactory = IP2DSpcNegBasicTrackGradeFactory,
                              trackGradeFactory = IP2DSpcNegDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DSpcNegTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcNegTagTool
