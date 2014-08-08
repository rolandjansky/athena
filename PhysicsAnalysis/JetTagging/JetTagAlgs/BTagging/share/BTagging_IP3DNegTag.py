# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DNegNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DNegNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DNegNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DNegNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DNegTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DNegTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DNegTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DNegTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DNeg = Analysis__SVForIPTool(name = "SVForIPTool_IP3DNeg",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DNeg
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DNeg

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DNegBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DNegBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DNegDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DNegDetailedTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       hitBLayerGrade=True,
                                                                       TrackSummaryTool = None,
                                                                       useDetailSharedHitInfo = True,
                                                                       useSharedHitInfo = True)

ToolSvc += IP3DNegBasicTrackGradeFactory
ToolSvc += IP3DNegDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DNegBasicTrackGradeFactory
  print IP3DNegDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DNegTagTool = Analysis__IPTag(
                              name = "IP3DNegTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DNegNewLikelihood,
                              trackSelectorTool = IP3DNegTrackSelector,
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
                              SVForIPTool = SVForIPTool_IP3DNeg,
#                              trackGradeFactory = IP3DNegBasicTrackGradeFactory,
                              trackGradeFactory = IP3DNegDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DNegTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DNegTagTool
