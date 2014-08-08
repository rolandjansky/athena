
from JetTagTools.JetTagToolsConf import Analysis__JetProbTag
JetProbFlipTagTool = Analysis__JetProbTag(
                              name = "JetProbFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              impactParameterView = "2D",
                              flipIPSign = True,
                              UseNegIP = True,
                              UsePosIP = False,
                              OutputLevel = BTaggingFlags.OutputLevel,
                              trackSelectorTool = JetProbTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
                              trackGradePartitions = [ "Good", "Shared" ],
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
ToolSvc += JetProbFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print JetProbFlipTagTool
