from JetTagTools.JetTagToolsConf import Analysis__MV1cTag
MV1cFlipTagTool = Analysis__MV1cTag(
                              name = "MV1cFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              inputIP3DWeightName = "IP3DNeg",
                              inputSV1WeightName = "SV1Flip",
                              inputJetFitterWeightName = "JetFitterCOMBNNIP3DNeg", 
                              OutputLevel = BTaggingFlags.OutputLevel
                             )
ToolSvc += MV1cFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MV1cFlipTagTool
