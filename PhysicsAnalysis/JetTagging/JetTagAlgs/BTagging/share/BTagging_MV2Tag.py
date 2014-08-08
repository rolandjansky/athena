from JetTagTools.JetTagToolsConf import Analysis__MV1Tag
MV2TagTool = Analysis__MV1Tag(
                              name = "MV2Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV2TagTool
if BTaggingFlags.OutputLevel < 3:
  print MV2TagTool
