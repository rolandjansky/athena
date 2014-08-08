# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DSpcNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DSpcNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DSpcNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DSpcTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DSpcTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DSpcTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2DSpc = Analysis__SVForIPTool(name = "SVForIPTool_IP2DSpc",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2DSpc
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2DSpc

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DSpcBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DSpcBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DSpcDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DSpcDetailedTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       hitBLayerGrade=True,
                                                                       TrackSummaryTool = None,
                                                                       useDetailSharedHitInfo = True,
                                                                       useSharedHitInfo = True)

ToolSvc += IP2DSpcBasicTrackGradeFactory
ToolSvc += IP2DSpcDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcBasicTrackGradeFactory
  print IP2DSpcDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DSpcTagTool = Analysis__IPTag(
                              name = "IP2DSpcTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DSpcNewLikelihood,
                              trackSelectorTool = IP2DSpcTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              SignWithSvx = True,
                              infoPlusName = "IPSpcInfoPlus",
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_IP2DSpc,
#                              trackGradeFactory = IP2DSpcBasicTrackGradeFactory,
                              trackGradeFactory = IP2DSpcDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DSpcTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DSpcTagTool
