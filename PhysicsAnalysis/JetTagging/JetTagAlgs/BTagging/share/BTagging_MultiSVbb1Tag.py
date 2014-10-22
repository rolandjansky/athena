  
from JetTagTools.JetTagToolsConf import Analysis__MultiSVTag
MultiSVbb1TagTool = Analysis__MultiSVTag(
                              name = "MultiSVbb1Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              #SVAlgType = "MSV",
                              xAODBaseName = "MSV",
                              taggerNameBase = "MultiSVbb1",
                              #infoPlusName = "MultiSVInfoPlus",
                              #originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
                              OutputLevel = BTaggingFlags.OutputLevel,
                              jetCollectionList = BTaggingFlags.Jets,
                              #jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              #SecVxFinderName   = "InDetVKalMultiVxInJetTool"
                              SecVxFinderName   = "MSV"
                             )
ToolSvc += MultiSVbb1TagTool
if BTaggingFlags.OutputLevel < 3:
  print MultiSVbb1TagTool
