from JetTagTools.JetTagToolsConf import Analysis__MV1cTag
MV1cTagTool = Analysis__MV1cTag(
                              name = "MV1cTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              xAODBaseName="MV1c",
                              inputIP3DWeightName="IP3D",
                              inputSV1WeightName="SV1",
                              inputJetFitterWeightName="JetFitterCombNN",
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV1cTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV1cTagTool
