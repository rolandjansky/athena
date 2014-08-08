# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DSpcNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DSpcNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DSpcNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DSpcTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DSpcTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DSpcTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3DSpc = Analysis__SVForIPTool(name = "SVForIPTool_IP3DSpc",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3DSpc
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3DSpc

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DSpcBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DSpcBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DSpcDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DSpcDetailedTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       hitBLayerGrade=True,
                                                                       TrackSummaryTool = None,
                                                                       useDetailSharedHitInfo = True,
                                                                       useSharedHitInfo = True)

ToolSvc += IP3DSpcBasicTrackGradeFactory
ToolSvc += IP3DSpcDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcBasicTrackGradeFactory
  print IP3DSpcDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DSpcTagTool = Analysis__IPTag(
                              name = "IP3DSpcTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DSpcNewLikelihood,
                              trackSelectorTool = IP3DSpcTrackSelector,
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
                              SVForIPTool = SVForIPTool_IP3DSpc,
#                              trackGradeFactory = IP3DSpcBasicTrackGradeFactory,
                              trackGradeFactory = IP3DSpcDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DSpcTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DSpcTagTool
