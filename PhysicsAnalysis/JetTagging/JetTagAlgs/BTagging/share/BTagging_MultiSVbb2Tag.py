  
from JetTagTools.JetTagToolsConf import Analysis__MultiSVTag
MultiSVbb2TagTool = Analysis__MultiSVTag(
                              name = "MultiSVbb2Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              #SVAlgType = "MSV",
                              xAODBaseName = "MSV",
                              taggerNameBase = "MultiSVbb2",
                              #infoPlusName = "MultiSVInfoPlus",
                              #originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              OutputLevel = BTaggingFlags.OutputLevel,
                              jetCollectionList = BTaggingFlags.Jets,
                              #jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              #SecVxFinderName   = "InDetVKalMultiVxInJetTool"
                              SecVxFinderName   = "MSV"
                             )
ToolSvc += MultiSVbb2TagTool
if BTaggingFlags.OutputLevel < 3:
  print MultiSVbb2TagTool
