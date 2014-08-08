# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
TrackCountingFlipTrackSelector = Analysis__TrackSelector( 
    name = "TrackCountingFlipTrackSelector",
    trackToVertexTool = BTagTrackToVertexTool,
    OutputLevel = BTaggingFlags.OutputLevel
    )
ToolSvc += TrackCountingFlipTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print TrackCountingFlipTrackSelector

from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
SVForIPTool_TrackCountingFlip = Analysis__SVForIPTool(name = "SVForIPTool_TrackCountingFlip",
                                                  OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += SVForIPTool_TrackCountingFlip
if BTaggingFlags.OutputLevel < 3:
  print SVForIPTool_TrackCountingFlip

from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
TrackCountingFlipBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "TrackCountingFlipBasicTrackGradeFactory",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       TrackSummaryTool = None,
                                                                       useSharedHitInfo = True)

ToolSvc += TrackCountingFlipBasicTrackGradeFactory
if BTaggingFlags.OutputLevel < 3:
  print TrackCountingFlipBasicTrackGradeFactory
                                                                   
from JetTagTools.JetTagToolsConf import Analysis__TrackCounting
TrackCountingFlipTool = Analysis__TrackCounting(
                              name = "TrackCountingFlip",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              trackSelectorTool = TrackCountingFlipTrackSelector,
                              trackToVertexTool = BTagTrackToVertexTool,
                              trackGradePartitions = [ "Good", "Shared" ],
                              RejectBadTracks = False,
                              SignWithSvx = False,
                              flipIPSign = True,
                              SecVxFinderNameForV0Removal = "InDetVKalVxInJetTool",
			      SecVxFinderNameForIPSign = "InDetVKalVxInJetTool",
                              SVForIPTool = SVForIPTool_TrackCountingFlip,
                              trackGradeFactory = TrackCountingFlipBasicTrackGradeFactory,
                              TrackToVertexIPEstimator = BTagTrackToVertexIPEstimator,
                              unbiasIPEstimation = False )
ToolSvc += TrackCountingFlipTool
if BTaggingFlags.OutputLevel < 3:
  print TrackCountingFlipTool
