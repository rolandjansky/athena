from JetTagTools.JetTagToolsConf import Analysis__MV3Tag
MV3_bVScTagTool = Analysis__MV3Tag(
                              name = "MV3_bVScTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV3CalibrationAlias = BTaggingFlags.ForceMV3CalibrationAlias,
                              MV3CalibAlias =  BTaggingFlags.MV3CalibAlias,
                              MV3Flavor = "bVSc",
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV3_bVScTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV3_bVScTagTool
