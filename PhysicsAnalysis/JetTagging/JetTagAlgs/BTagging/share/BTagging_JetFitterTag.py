from AthenaCommon.AppMgr import ToolSvc

from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
jetFitterNtupleWriter = Analysis__JetFitterNtupleWriter( name = "JetFitterNtupleWriter",
                                                         OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += jetFitterNtupleWriter
if BTaggingFlags.OutputLevel < 3:
  print jetFitterNtupleWriter   


from JetTagTools.JetTagToolsConf import Analysis__JetFitterLikelihood
jetFitterLikelihood = Analysis__JetFitterLikelihood( name = "JetFitterLikelihood",
						     CalibrationDirectory = "JetFitter",
					 	     CalibrationSubDirectory = "RooFitLikelihood",
						     calibrationTool = BTagCalibrationBrokerTool,
                                                     OutputLevel = BTaggingFlags.OutputLevel )
ToolSvc += jetFitterLikelihood
if BTaggingFlags.OutputLevel < 3:
  print jetFitterLikelihood

from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
JetFitterTagTool = Analysis__JetFitterTag( name = "JetFitterTag",
                                           OutputLevel = BTaggingFlags.OutputLevel,
                                           Runmodus = BTaggingFlags.Runmodus,
                                           jetCollectionList = BTaggingFlags.Jets,
                                           jetfitterNtupleWriter = jetFitterNtupleWriter,
                                           jetfitterClassifier = jetFitterLikelihood,
                                           jetfitterVariablesFactory = JetFitterVariablesFactory,
                                           useForcedCalibration = False,
                                           storeOnlyBaseObject = False
                                           )

ToolSvc += JetFitterTagTool

if BTaggingFlags.OutputLevel < 3:
  print JetFitterTagTool
