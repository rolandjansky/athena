#load the VKalVertex in Jet finder tool

from AthenaCommon.AppMgr import ToolSvc
from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
InDetVKalVxInJetTool = InDet__InDetVKalVxInJetTool(name             = "InDetVKalVxInJetTool",
#                                                   TrackSummaryTool = None, #Do not need it in b-tagging
                                                   OutputLevel      = BTaggingFlags.OutputLevel)


ToolSvc += InDetVKalVxInJetTool
if BTaggingFlags.OutputLevel < 3:
  print InDetVKalVxInJetTool
