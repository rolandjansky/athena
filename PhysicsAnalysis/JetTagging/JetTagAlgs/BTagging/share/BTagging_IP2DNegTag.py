# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DNegNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DNegNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DNegNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DNegNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DNegTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DNegTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DNegTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DNegTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DNeg = Analysis__SVForIPTool(name = "SVForIPTool_IP2DNeg",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DNeg
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DNeg

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DNegBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DNegBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DNegDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DNegDetailedTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       hitBLayerGrade=True,
                                                                       TrackSummaryTool = None,
                                                                       useDetailSharedHitInfo = True,
                                                                       useSharedHitInfo = True)

ToolSvc += IP2DNegBasicTrackGradeFactory
ToolSvc += IP2DNegDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DNegBasicTrackGradeFactory
  print IP2DNegDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DNegTagTool = Analysis__IPTag(
                              name = "IP2DNegTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DNegNewLikelihood,
                              trackSelectorTool = IP2DNegTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              usePosIP = True,
                              useNegIP = False,
                              flipIPSign = True,
                              writeInfoPlus = False, 
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DNeg,
#                              trackGradeFactory = IP2DNegBasicTrackGradeFactory,
                              trackGradeFactory = IP2DNegDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DNegTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DNegTagTool
