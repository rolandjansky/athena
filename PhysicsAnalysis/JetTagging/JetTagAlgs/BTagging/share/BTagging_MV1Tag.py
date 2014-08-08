from JetTagTools.JetTagToolsConf import Analysis__MV1Tag
MV1TagTool = Analysis__MV1Tag(
                              name = "MV1Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              xAODBaseName="MV1",
                              inputIP3DWeightName="IP3D",
                              inputSV1WeightName="SV1",
                              inputJetFitterWeightName="JetFitterCombNN",
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV1TagTool
if BTaggingFlags.OutputLevel < 3:
  print MV1TagTool
