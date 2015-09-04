# Track selector tool
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
GbbNNTagTrackSelector = Analysis__TrackSelector( 
    name = "GbbNNTagTrackSelector",
    trackToVertexTool = BTagTrackToVertexTool,
    OutputLevel = BTaggingFlags.OutputLevel
    )
ToolSvc += GbbNNTagTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print GbbNNTagTrackSelector

#
from JetTagTools.JetTagToolsConf import Analysis__GbbNNTag
GbbNNTag = Analysis__GbbNNTag( name = "GbbNNTag",
                                   OutputLevel = BTaggingFlags.OutputLevel,
                                   Runmodus = BTaggingFlags.Runmodus,
                                   trackSelectorTool = GbbNNTagTrackSelector,
				   NNweightsFileName = "GbbNNweightsFile.txt",
                                   )

ToolSvc += GbbNNTag

if BTaggingFlags.OutputLevel < 3:
  print GbbNNTag


