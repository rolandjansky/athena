# each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
JetProbTrackSelector = Analysis__TrackSelector( 
                                         name = "JetProbTrackSelector",
                                         OutputLevel = BTaggingFlags.OutputLevel,
                                         trackToVertexTool = BTagTrackToVertexTool
                                         )
ToolSvc += JetProbTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print JetProbTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_JetProb = Analysis__SVForIPTool(name = "SVForIPTool_JetProb",
                                            OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_JetProb
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_JetProb

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
JetProbBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "JetProbBasicTrackGradeFactory",
                                                              OutputLevel = BTaggingFlags.OutputLevel,
                                                              TrackSummaryTool = None,
                                                              useSharedHitInfo = True)

ToolSvc += JetProbBasicTrackGradeFactory
if BTaggingFlags.OutputLevel < 3:
  print JetProbBasicTrackGradeFactory

from JetTagTools.JetTagToolsConf import Analysis__JetProbTag
JetProbTagTool = Analysis__JetProbTag(
                              name = "JetProbTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              impactParameterView = "2D",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              trackSelectorTool = JetProbTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
                              trackGradePartitions = [ "Good", "Shared" ],
                              UseNegIP = False,
                              RejectBadTracks = False,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_JetProb,
                              calibrationTool = BTagCalibrationBrokerTool,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              trackGradeFactory = JetProbBasicTrackGradeFactory,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False, #switch to true (better!) when creating new PDFs
                              InputType = globalflags.DataSource()       
                              )
ToolSvc += JetProbTagTool
if BTaggingFlags.OutputLevel < 3:
  print JetProbTagTool
