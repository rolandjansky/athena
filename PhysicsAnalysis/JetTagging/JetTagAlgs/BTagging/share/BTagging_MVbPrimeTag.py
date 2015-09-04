from JetTagTools.JetTagToolsConf import Analysis__MVbPrimeTag
MVbPrimeTagTool = Analysis__MVbPrimeTag(
                              name = "MVbPrimeTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              calibrationTool = BTagCalibrationBrokerTool,
                              OutputLevel = BTaggingFlags.OutputLevel
                             )

ToolSvc += MVbPrimeTagTool
if BTaggingFlags.OutputLevel < 3:
  print MVbPrimeTagTool

#print "btag calib tag:", BTaggingFlags.CalibrationTag
