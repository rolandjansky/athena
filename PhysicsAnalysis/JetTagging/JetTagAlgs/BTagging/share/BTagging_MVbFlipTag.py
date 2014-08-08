from JetTagTools.JetTagToolsConf import Analysis__MVbTag
MVbFlipTagTool = Analysis__MVbTag(
                              name = "MVbFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              #inputIP3DWeightName = "IP3DNeg",
                              #inputSV1WeightName = "SV1Flip",
                              #inputJetFitterWeightName = "JetFitterCOMBNNIP3DNeg", 
                              OutputLevel = BTaggingFlags.OutputLevel,
                              flipMVb = True
                             )
ToolSvc += MVbFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MVbFlipTagTool
