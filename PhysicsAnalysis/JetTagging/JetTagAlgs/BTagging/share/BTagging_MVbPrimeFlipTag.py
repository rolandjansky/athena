from JetTagTools.JetTagToolsConf import Analysis__MVbPrimeTag
MVbPrimeFlipTagTool = Analysis__MVbPrimeTag(
                              name = "MVbPrimeFlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              #inputIP3DWeightName = "IP3DNeg",
                              #inputSV1WeightName = "SV1Flip",
                              #inputJetFitterWeightName = "JetFitterCOMBNNIP3DNeg", 
                              OutputLevel = BTaggingFlags.OutputLevel,
                              flipMVbPrime = True
                             )
ToolSvc += MVbPrimeFlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print MVbPrimeFlipTagTool
