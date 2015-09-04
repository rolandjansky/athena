#load the VKalVertex in Jet finder tool, configured to keep also vertices with negative Lxy

from AthenaCommon.AppMgr import ToolSvc
from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
InDetVKalVxNegInJetTool = InDet__InDetVKalVxInJetTool(name             = "InDetVKalVxNegInJetTool",
                                                      getNegativeTail  = True,
                                                      CutA0 = 2.0, # new tuned cuts for SV0 (Sara)
                                                      CutZVrt = 2.0,
                                                      CutPixelHits = 2,
                                                      OutputLevel      = BTaggingFlags.OutputLevel)

ToolSvc += InDetVKalVxNegInJetTool
if BTaggingFlags.OutputLevel < 3:
  print InDetVKalVxNegInJetTool
