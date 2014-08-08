from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
MV2c00FlipTagTool = Analysis__MV2Tag(
                              name = "MV2c00FlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV2CalibrationAlias = BTaggingFlags.ForceMV2CalibrationAlias,
                              MV2CalibAlias =  BTaggingFlags.MV2CalibAlias,
                              OutputLevel = BTaggingFlags.OutputLevel,
                              flipMV2 = True
                             )
ToolSvc += MV2c00FlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV2c00FlipTagTool
