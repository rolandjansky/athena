# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DPosNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DPosNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DPosNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DPosNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DPosTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DPosTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DPosTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DPosTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DPos = Analysis__SVForIPTool(name = "SVForIPTool_IP2DPos",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DPos
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DPos

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DPosBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DPosBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DPosDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DPosDetailedTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       hitBLayerGrade=True,
                                                                       TrackSummaryTool = None,
                                                                       useDetailSharedHitInfo = True,
                                                                       useSharedHitInfo = True)

ToolSvc += IP2DPosBasicTrackGradeFactory
ToolSvc += IP2DPosDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DPosBasicTrackGradeFactory
  print IP2DPosDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DPosTagTool = Analysis__IPTag(
                              name = "IP2DPosTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DPosNewLikelihood,
                              trackSelectorTool = IP2DPosTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              usePosIP = True,
                              useNegIP = False,
                              flipIPSign = False,
                              writeInfoPlus = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DPos,
#                              trackGradeFactory = IP2DPosBasicTrackGradeFactory,
                              trackGradeFactory = IP2DPosDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DPosTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DPosTagTool
