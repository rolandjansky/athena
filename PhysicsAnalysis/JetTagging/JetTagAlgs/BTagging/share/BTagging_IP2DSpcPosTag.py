# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DSpcPosNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DSpcPosNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DSpcPosNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcPosNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DSpcPosTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DSpcPosTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DSpcPosTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcPosTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DSpcPos = Analysis__SVForIPTool(name = "SVForIPTool_IP2DSpcPos",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DSpcPos
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DSpcPos

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DSpcPosBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DSpcPosBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DSpcPosDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DSpcPosDetailedTrackGradeFactory",
                                                                          OutputLevel = BTaggingFlags.OutputLevel,
                                                                          hitBLayerGrade=True,
                                                                          TrackSummaryTool = None,
                                                                          useDetailSharedHitInfo = True,
                                                                          useSharedHitInfo = True)

ToolSvc += IP2DSpcPosBasicTrackGradeFactory
ToolSvc += IP2DSpcPosDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcPosBasicTrackGradeFactory
  print IP2DSpcPosDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DSpcPosTagTool = Analysis__IPTag(
                              name = "IP2DSpcPosTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DSpcPosNewLikelihood,
                              trackSelectorTool = IP2DSpcPosTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              usePosIP = True,
                              useNegIP = False,
                              flipIPSign = False,
                              writeInfoPlus = False,
                              SignWithSvx = True,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DSpcPos,
#                              trackGradeFactory = IP2DSpcPosBasicTrackGradeFactory,
                              trackGradeFactory = IP2DSpcPosDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DSpcPosTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcPosTagTool
