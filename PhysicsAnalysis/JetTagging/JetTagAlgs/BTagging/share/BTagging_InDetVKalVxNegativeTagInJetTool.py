#load the VKalVertex in Jet finder tool, configured to keep also vertices with negative Lxy

from AthenaCommon.AppMgr import ToolSvc
from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
InDetVKalVxNegativeTagInJetTool = InDet__InDetVKalVxInJetTool(name             = "InDetVKalVxNegativeTagInJetTool",
                                                      getNegativeTag  = True,
                                                      CutA0 = 2.0, # new tuned cuts for SV0 (Sara)
                                                      CutZVrt = 2.0,
                                                      CutPixelHits = 2,
                                                      TrackSummaryTool = None, #Do not need it in b-tagging
                                                      OutputLevel      = BTaggingFlags.OutputLevel)

ToolSvc += InDetVKalVxNegativeTagInJetTool
if BTaggingFlags.OutputLevel < 3:
  print InDetVKalVxNegativeTagInJetTool
