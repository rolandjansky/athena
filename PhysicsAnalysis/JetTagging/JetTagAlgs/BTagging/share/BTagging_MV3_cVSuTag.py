from JetTagTools.JetTagToolsConf import Analysis__MV3Tag
MV3_cVSuTagTool = Analysis__MV3Tag(
                              name = "MV3_cVSuTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV3CalibrationAlias = BTaggingFlags.ForceMV3CalibrationAlias,
                              MV3CalibAlias =  BTaggingFlags.MV3CalibAlias,
                              MV3Flavor = "cVSu",
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV3_cVSuTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV3_cVSuTagTool
