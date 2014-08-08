from JetTagTools.JetTagToolsConf import Analysis__MV3Tag
MV3_bVScFlipTagTool = Analysis__MV3Tag(
                              name = "MV3_bVScFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV3CalibrationAlias = BTaggingFlags.ForceMV3CalibrationAlias,
                              MV3CalibAlias =  BTaggingFlags.MV3CalibAlias,
                              MV3Flavor = "bVSc",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              flipMV3 = True
                             )
ToolSvc += MV3_bVScFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV3_bVScFlipTagTool
