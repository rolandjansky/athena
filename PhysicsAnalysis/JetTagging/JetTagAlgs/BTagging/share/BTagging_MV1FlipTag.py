from JetTagTools.JetTagToolsConf import Analysis__MV1Tag
MV1FlipTagTool = Analysis__MV1Tag(
                              name = "MV1FlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              inputIP3DWeightName = "IP3DNeg",
                              inputSV1WeightName = "SV1Flip",
                              inputJetFitterWeightName = "JetFitterCOMBNNIP3DNeg", 
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV1FlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV1FlipTagTool
