# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
IP3DNewLikelihood = Analysis__NewLikelihoodTool( 
                                                 name = "IP3DNewLikelihoodTool",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 taggerName = "IP3D",
                                                 calibrationTool = BTagCalibrationBrokerTool
                                               )
ToolSvc += IP3DNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print IP3DNewLikelihood

# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print IP3DTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_IP3D = Analysis__SVForIPTool(name = "SVForIPTool_IP3D",
                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_IP3D
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_IP3D

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
IP3DBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
#                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
IP3DDetailedTrackGradeFactory = Analysis__DetailedTrackGradeFactory(name = "IP3DDetailedTrackGradeFactory",
                                                                    OutputLevel = BTaggingFlags.OutputLevel,
                                                                    hitBLayerGrade=True,
#                                                                    TrackSummaryTool = None,
                                                                    useDetailSharedHitInfo = True,
                                                                    useSharedHitInfo = True)


ToolSvc += IP3DBasicTrackGradeFactory
ToolSvc += IP3DDetailedTrackGradeFactory

if BTaggingFlags.OutputLevel < 3:
  print IP3DBasicTrackGradeFactory
  print IP3DDetailedTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__IPTag
IP3DTagTool = Analysis__IPTag(
                              name = "IP3DTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              impactParameterView = "3D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool = IP3DNewLikelihood,
                              trackSelectorTool = IP3DTrackSelector,
#                              trackToVertexTool = BTagTrackToVertexTool,
#                              trackGradePartitions = [ "Good", "Shared" ],
                              trackGradePartitions = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                              RejectBadTracks = False,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              jetCollectionList = BTaggingFlags.Jets, # not used
#                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
#                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
#                              SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SecVxFinderName = "SV1",
                              SVForIPTool = SVForIPTool_IP3D,
#                              trackGradeFactory = IP3DBasicTrackGradeFactory,
                              trackGradeFactory = IP3DDetailedTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              UseCHypo=True,
                              xAODBaseName="IP3D",
                              unbiasIPEstimation = False #switch to true (better!) when creating new PDFs
                             )
ToolSvc += IP3DTagTool
if BTaggingFlags.OutputLevel < 3:
  print IP3DTagTool
