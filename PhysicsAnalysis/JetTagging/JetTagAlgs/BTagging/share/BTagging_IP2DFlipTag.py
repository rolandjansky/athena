# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DFlipNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DFlipNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DFlipNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DFlipNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DFlipTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DFlipTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DFlipTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DFlipTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DFlip = Analysis__SVForIPTool(name = "SVForIPTool_IP2DFlip",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DFlip
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DFlip

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DFlipBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DFlipBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DFlipDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DFlipDetailedTrackGradeFactory",
                                                                        OutputLevel = BTaggingFlags.OutputLevel,
                                                                        hitBLayerGrade=True,
                                                                        TrackSummaryTool = None,
                                                                        useDetailSharedHitInfo = True,
                                                                        useSharedHitInfo = True)

ToolSvc += IP2DFlipBasicTrackGradeFactory
ToolSvc += IP2DFlipDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DFlipBasicTrackGradeFactory
  print IP2DFlipDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DFlipTagTool = Analysis__IPTag(
                              name = "IP2DFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DFlipNewLikelihood,
                              trackSelectorTool = IP2DFlipTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              flipIPSign = True,
                              writeInfoPlus = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DFlip,
#                              trackGradeFactory = IP2DFlipBasicTrackGradeFactory,
                              trackGradeFactory = IP2DFlipDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )

ToolSvc += IP2DFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DFlipTagTool
