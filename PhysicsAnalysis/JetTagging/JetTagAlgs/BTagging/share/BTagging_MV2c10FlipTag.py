from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
MV2c10FlipTagTool = Analysis__MV2Tag(
                              name = "MV2c10FlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV2CalibrationAlias = BTaggingFlags.ForceMV2CalibrationAlias,
                              MV2CalibAlias =  BTaggingFlags.MV2CalibAlias,
                              OutputLevel = BTaggingFlags.OutputLevel,
                              flipMV2 = True
                             )
ToolSvc += MV2c10FlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV2c10FlipTagTool
