#load the VKalVertex in Jet finder tool, configured to keep also vertices with negative Lxy

from AthenaCommon.AppMgr import ToolSvc
#from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import AtlasVKalVxInJetFinder
#InDetVKalMultiVxInJetTool = InDet__InDetVKalVxInJetTool(name             = "InDetVKalMultiVxInJetTool",
InDetVKalMultiVxInJetTool = AtlasVKalVxInJetFinder(name             = "InDetVKalMultiVxInJetTool")

InDetVKalMultiVxInJetTool.getNegativeTail  = False
InDetVKalMultiVxInJetTool.ConeForTag = 1.
InDetVKalMultiVxInJetTool.MultiVertex = True
InDetVKalMultiVxInJetTool.OutputLevel      = BTaggingFlags.OutputLevel

ToolSvc += InDetVKalMultiVxInJetTool
if BTaggingFlags.OutputLevel < 3:
  print InDetVKalMultiVxInJetTool

## factory for BTagSecVertexing
from JetTagTools.JetTagToolsConf import Analysis__MSVVariablesFactory
MSVVariablesFactory = Analysis__MSVVariablesFactory( name = "MSVVariablesFactory",
                                                     OutputLevel = BTaggingFlags.OutputLevel)
ToolSvc += MSVVariablesFactory

if BTaggingFlags.OutputLevel < 3:
  print MSVVariablesFactory
