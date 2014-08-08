# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DSpcFlipNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DSpcFlipNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DSpcFlipNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcFlipNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DSpcFlipTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DSpcFlipTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DSpcFlipTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcFlipTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DSpcFlip = Analysis__SVForIPTool(name = "SVForIPTool_IP3DSpcFlip",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DSpcFlip
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DSpcFlip

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DSpcFlipBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DSpcFlipBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DSpcFlipDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DSpcFlipDetailedTrackGradeFactory",
                                                                           OutputLevel = BTaggingFlags.OutputLevel,
                                                                           hitBLayerGrade=True,
                                                                           TrackSummaryTool = None,
                                                                           useDetailSharedHitInfo = True,
                                                                           useSharedHitInfo = True)

ToolSvc += IP3DSpcFlipBasicTrackGradeFactory
ToolSvc += IP3DSpcFlipDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcFlipBasicTrackGradeFactory
  print IP3DSpcFlipDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DSpcFlipTagTool = Analysis__IPTag(
                              name = "IP3DSpcFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DSpcFlipNewLikelihood,
                              trackSelectorTool = IP3DSpcFlipTrackSelector,
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
                              SVForIPTool = SVForIPTool_IP3DSpcFlip,
#                              trackGradeFactory = IP3DSpcFlipBasicTrackGradeFactory,
                              trackGradeFactory = IP3DSpcFlipDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DSpcFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcFlipTagTool
