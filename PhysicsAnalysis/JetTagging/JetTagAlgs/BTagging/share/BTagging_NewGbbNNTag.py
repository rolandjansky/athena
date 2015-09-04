# Track selector tool
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
GbbNNTagTrackSelector = Analysis__TrackSelector( 
    name = "GbbNNTagTrackSelector",
    trackToVertexTool = BTagTrackToVertexTool,
    pTMin = 1000.,
    d0Max = 2.0,
    z0Max = 2.0,
    nHitPix = 1,
    nHitSct = 4,
    nHitSi = 7,
    fitChi2OnNdfMax = 3.0,
    OutputLevel = BTaggingFlags.OutputLevel
    )
ToolSvc += GbbNNTagTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print GbbNNTagTrackSelector

#
from JetTagTools.JetTagToolsConf import Analysis__HadFlavorTag
NewGbbNNTag = Analysis__HadFlavorTag( name = "NewGbbNNTag",
                                      OutputLevel = BTaggingFlags.OutputLevel,
                                      tagMode = "GbbNN",
                                      Runmodus = BTaggingFlags.Runmodus,
                                      trackSelectorTool = GbbNNTagTrackSelector,
                                      )

ToolSvc += NewGbbNNTag

if BTaggingFlags.OutputLevel < 3:
  print NewGbbNNTag


