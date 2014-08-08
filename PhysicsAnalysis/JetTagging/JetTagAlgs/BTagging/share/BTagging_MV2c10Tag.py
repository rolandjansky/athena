from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
MV2c10TagTool = Analysis__MV2Tag(
                              name = "MV2c10Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              forceMV2CalibrationAlias = BTaggingFlags.ForceMV2CalibrationAlias,
                              MV2CalibAlias =  BTaggingFlags.MV2CalibAlias,
                              xAODBaseName="MV2c10",
                              inputSV0SourceName = "SV0",
                              inputSV1SourceName = "SV1",
                              inputIP2DSourceName = "IP2D",
                              inputIP3DSourceName = "IP3D",
                              inputJFSourceName = "JetFitter",
                              inputJFProbSourceName = "JetFitterCombNN",
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV2c10TagTool
if BTaggingFlags.OutputLevel < 3:
  print MV2c10TagTool
