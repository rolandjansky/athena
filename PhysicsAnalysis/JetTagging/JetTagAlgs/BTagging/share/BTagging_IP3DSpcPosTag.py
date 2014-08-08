# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DSpcPosNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DSpcPosNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DSpcPosNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcPosNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DSpcPosTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DSpcPosTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DSpcPosTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcPosTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DSpcPos = Analysis__SVForIPTool(name = "SVForIPTool_IP3DSpcPos",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DSpcPos
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DSpcPos

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DSpcPosBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DSpcPosBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DSpcPosDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DSpcPosDetailedTrackGradeFactory",
                                                                          OutputLevel = BTaggingFlags.OutputLevel,
                                                                          hitBLayerGrade=True,
                                                                          TrackSummaryTool = None,
                                                                          useDetailSharedHitInfo = True,
                                                                          useSharedHitInfo = True)

ToolSvc += IP3DSpcPosBasicTrackGradeFactory
ToolSvc += IP3DSpcPosDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcPosBasicTrackGradeFactory
  print IP3DSpcPosDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DSpcPosTagTool = Analysis__IPTag(
                              name = "IP3DSpcPosTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DSpcPosNewLikelihood,
                              trackSelectorTool = IP3DSpcPosTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              usePosIP = True,
                              useNegIP = False,
                              flipIPSign = False,
                              SignWithSvx = True,
                              writeInfoPlus = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP3DSpcPos,
#                              trackGradeFactory = IP3DSpcPosBasicTrackGradeFactory,
                              trackGradeFactory = IP3DSpcPosDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DSpcPosTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcPosTagTool
