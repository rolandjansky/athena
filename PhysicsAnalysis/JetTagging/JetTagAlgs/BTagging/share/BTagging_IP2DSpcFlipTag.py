# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DSpcFlipNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DSpcFlipNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DSpcFlipNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcFlipNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DSpcFlipTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DSpcFlipTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DSpcFlipTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcFlipTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DSpcFlip = Analysis__SVForIPTool(name = "SVForIPTool_IP2DSpcFlip",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DSpcFlip
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DSpcFlip

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DSpcFlipBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DSpcFlipBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DSpcFlipDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DSpcFlipDetailedTrackGradeFactory",
                                                                           OutputLevel = BTaggingFlags.OutputLevel,
                                                                           hitBLayerGrade=True,
                                                                           TrackSummaryTool = None,
                                                                           useDetailSharedHitInfo = True,
                                                                           useSharedHitInfo = True)

ToolSvc += IP2DSpcFlipBasicTrackGradeFactory
ToolSvc += IP2DSpcFlipDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcFlipBasicTrackGradeFactory
  print IP2DSpcFlipDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DSpcFlipTagTool = Analysis__IPTag(
                              name = "IP2DSpcFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DSpcFlipNewLikelihood,
                              trackSelectorTool = IP2DSpcFlipTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              SignWithSvx = True,
                              flipIPSign = True,
                              writeInfoPlus = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DSpcFlip,
#                              trackGradeFactory = IP2DSpcFlipBasicTrackGradeFactory,
                              trackGradeFactory = IP2DSpcFlipDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DSpcFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcFlipTagTool
