# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP2DNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP2DNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP2D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP2DNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP2DNewLikelihood


# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP2DTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP2DTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP2DTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP2DTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP2D = Analysis__SVForIPTool(name = "SVForIPTool_IP2D",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP2D
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP2D

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP2DBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP2DBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              #TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP2DDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP2DDetailedTrackGradeFactory",
                                                                    OutputLevel = BTaggingFlags.OutputLevel,
                                                                    hitBLayerGrade=True,
                                                                    #TrackSummaryTool = None,
                                                                    useDetailSharedHitInfo = True,
                                                                    useSharedHitInfo = True)

ToolSvc += IP2DBasicTrackGradeFactory
ToolSvc += IP2DDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP2DBasicTrackGradeFactory
  print IP2DDetailedTrackGradeFactory
                                                                   

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP2DTagTool = Analysis__IPTag(
                              name = "IP2DTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP2DNewLikelihood,
                              trackSelectorTool = IP2DTrackSelector,
                              #trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets,
                              #jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              #SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderName = "SV1",
                              SVForIPTool = SVForIPTool_IP2D,
#                              trackGradeFactory = IP2DBasicTrackGradeFactory,
                              trackGradeFactory = IP2DDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              xAODBaseName="IP2D",
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP2DTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP2DTagTool
