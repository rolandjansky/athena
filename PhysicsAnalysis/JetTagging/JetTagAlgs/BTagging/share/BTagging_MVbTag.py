from JetTagTools.JetTagToolsConf import Analysis__MVbTag
MVbTagTool = Analysis__MVbTag(
                              name = "MVbTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              xAODBaseName="MVb",
                              inputSV0SourceName = "SV0",
                              inputSV1SourceName = "SV1",
                              inputIP2DSourceName = "IP2D",
                              inputIP3DSourceName = "IP3D",
                              inputJFSourceName = "JetFitter",
                              inputJFProbSourceName = "JetFitterCombNN",
                              OutputLevel = BTaggingFlags.OutputLevel
                             )

ToolSvc += MVbTagTool
if BTaggingFlags.OutputLevel < 3:
  print MVbTagTool

#print "btag calib tag:", BTaggingFlags.CalibrationTag
