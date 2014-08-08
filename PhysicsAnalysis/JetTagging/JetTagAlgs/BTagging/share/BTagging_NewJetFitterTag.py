from AthenaCommon.AppMgr import ToolSvc

from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
NewJetFitterNtupleWriter = Analysis__JetFitterNtupleWriter( name = "NewJetFitterNtupleWriter",
                                                            OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += NewJetFitterNtupleWriter
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterNtupleWriter   

from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
NewJetFitterVariablesFactory = Analysis__JetFitterVariablesFactory( name = "NewJetFitterVariablesFactory",
                                                                    OutputLevel = BTaggingFlags.OutputLevel,
                                                                    JetFitterInstance = "JetFitterTag",
                                                                    usePtCorrectedMass = True)	
ToolSvc += NewJetFitterVariablesFactory
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterVariablesFactory


from JetTagTools.JetTagToolsConf import Analysis__JetFitterLikelihood
NewJetFitterLikelihood = Analysis__JetFitterLikelihood( name = "NewJetFitterLikelihood",
						        CalibrationDirectory = "JetFitter",
							CalibrationSubDirectory = "RooFitLikelihood",
							calibrationTool = BTagCalibrationBrokerTool,
                                                        OutputLevel = BTaggingFlags.OutputLevel )
ToolSvc += NewJetFitterLikelihood
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterLikelihood

from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
NewJetFitterTagTool = Analysis__JetFitterTag( name = "NewJetFitterTag",
                                              OutputLevel = BTaggingFlags.OutputLevel,
                                              Runmodus = BTaggingFlags.Runmodus,
                                              jetfitterNtupleWriter = NewJetFitterNtupleWriter,
                                              jetfitterLikelihood = NewJetFitterLikelihood,
                                              jetCollectionList = BTaggingFlags.Jets,
                                              jetfitterVariablesFactory = NewJetFitterVariablesFactory,
                                              secVxFinderName = "NewJetFitterVxFinder",
                                              useForcedCalibration = False
                                              )

ToolSvc += NewJetFitterTagTool


if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterTagTool
