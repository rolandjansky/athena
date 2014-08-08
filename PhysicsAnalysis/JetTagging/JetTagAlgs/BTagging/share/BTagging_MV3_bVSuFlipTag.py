from JetTagTools.JetTagToolsConf import Analysis__MV3Tag
MV3_bVSuFlipTagTool = Analysis__MV3Tag(
                              name = "MV3_bVSuFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV3CalibrationAlias = BTaggingFlags.ForceMV3CalibrationAlias,
                              MV3CalibAlias =  BTaggingFlags.MV3CalibAlias,
                              MV3Flavor = "bVSu",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              flipMV3 = True
                             )
ToolSvc += MV3_bVSuFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV3_bVSuFlipTagTool
