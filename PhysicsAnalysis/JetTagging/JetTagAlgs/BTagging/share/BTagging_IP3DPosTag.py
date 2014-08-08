# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DPosNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DPosNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DPosNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DPosNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DPosTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DPosTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DPosTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DPosTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DPos = Analysis__SVForIPTool(name = "SVForIPTool_IP3DPos",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DPos
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DPos

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DPosBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DPosBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DPosDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DPosDetailedTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       hitBLayerGrade=True,
                                                                       TrackSummaryTool = None,
                                                                       useDetailSharedHitInfo = True,
                                                                       useSharedHitInfo = True)

ToolSvc += IP3DPosBasicTrackGradeFactory
ToolSvc += IP3DPosDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DPosBasicTrackGradeFactory
  print IP3DPosDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DPosTagTool = Analysis__IPTag(
                              name = "IP3DPosTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DPosNewLikelihood,
                              trackSelectorTool = IP3DPosTrackSelector,
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
                              SVForIPTool = SVForIPTool_IP3DPos,
#                              trackGradeFactory = IP3DPosBasicTrackGradeFactory,
                              trackGradeFactory = IP3DPosDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DPosTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DPosTagTool
