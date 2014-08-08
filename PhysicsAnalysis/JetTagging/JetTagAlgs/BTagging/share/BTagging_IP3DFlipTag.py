# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DFlipNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DFlipNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DFlipNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DFlipNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DFlipTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DFlipTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DFlipTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DFlipTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DFlip = Analysis__SVForIPTool(name = "SVForIPTool_IP3DFlip",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DFlip
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DFlip

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DFlipBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DFlipBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DFlipDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DFlipDetailedTrackGradeFactory",
                                                                        OutputLevel = BTaggingFlags.OutputLevel,
                                                                        hitBLayerGrade=True,
                                                                        TrackSummaryTool = None,
                                                                        useDetailSharedHitInfo = True,
                                                                        useSharedHitInfo = True)

ToolSvc += IP3DFlipBasicTrackGradeFactory
ToolSvc += IP3DFlipDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DFlipBasicTrackGradeFactory
  print IP3DFlipDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DFlipTagTool = Analysis__IPTag(
                              name = "IP3DFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DFlipNewLikelihood,
                              trackSelectorTool = IP3DFlipTrackSelector,
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
                              SVForIPTool = SVForIPTool_IP3DFlip,
#                              trackGradeFactory = IP3DFlipBasicTrackGradeFactory,
                              trackGradeFactory = IP3DFlipDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DFlipTagTool
