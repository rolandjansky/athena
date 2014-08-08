  
from JetTagTools.JetTagToolsConf import Analysis__SVTag
SV0TagTool = Analysis__SVTag(
                              name = "SV0Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              SVAlgType = "SV0",
                              SecVxFinderName   = "SV0",
                              xAODBaseName = "SV0",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              jetCollectionList = BTaggingFlags.Jets
                             )
ToolSvc += SV0TagTool
if BTaggingFlags.OutputLevel < 3:
  print SV0TagTool
