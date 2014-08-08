  
from JetTagTools.JetTagToolsConf import Analysis__MultiSVTag
MultiSVTagTool = Analysis__MultiSVTag(
                              name = "MutliSVTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              infoPlusName = "MultiSVInfoPlus",
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              OutputLevel = BTaggingFlags.OutputLevel,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              SecVxFinderName   = "InDetVKalMultiVxInJetTool"
                             )
ToolSvc += MultiSVTagTool
if BTaggingFlags.OutputLevel < 3:
  print MultiSVTagTool
