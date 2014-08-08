# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
TrackCountingTrackSelector = Analysis__TrackSelector( 
    name = "TrackCountingTrackSelector",
    trackToVertexTool = BTagTrackToVertexTool,
    OutputLevel = BTaggingFlags.OutputLevel
    )
ToolSvc += TrackCountingTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print TrackCountingTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_TrackCounting = Analysis__SVForIPTool(name = "SVForIPTool_TrackCounting",
                                                  OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_TrackCounting
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_TrackCounting

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
TrackCountingBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "TrackCountingBasicTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       TrackSummaryTool = None,
                                                                       useSharedHitInfo = True)

ToolSvc += TrackCountingBasicTrackGradeFactory
if BTaggingFlags.OutputLevel < 3:
  print TrackCountingBasicTrackGradeFactory
                                                                   
from JetTagTools.JetTagToolsConf import Analysis__TrackCounting
TrackCountingTool = Analysis__TrackCounting(
                              name = "TrackCounting",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              trackSelectorTool = TrackCountingTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
                              trackGradePartitions = [ "Good", "Shared" ],
                              RejectBadTracks = False,
                              SignWithSvx = False,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_TrackCounting,
                              trackGradeFactory = TrackCountingBasicTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False )
ToolSvc += TrackCountingTool
if BTaggingFlags.OutputLevel < 3:
  print TrackCountingTool
