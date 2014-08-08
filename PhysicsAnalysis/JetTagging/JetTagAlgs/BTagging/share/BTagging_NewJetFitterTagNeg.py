from AthenaCommon.AppMgr import ToolSvc

if (not BTaggingFlags.JetFitterTag):
  from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
  NewJetFitterNtupleWriter = Analysis__JetFitterNtupleWriter( name = "NewJetFitterNtupleWriter",
                                                              OutputLevel = BTaggingFlags.OutputLevel)

  ToolSvc += NewJetFitterNtupleWriter
  if BTaggingFlags.OutputLevel < 3:
    print NewJetFitterNtupleWriter   

from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
NewJetFitterVariablesFactoryFlip = Analysis__JetFitterVariablesFactory( name = "NewJetFitterVariablesFactoryFlip",
                                                                        OutputLevel = BTaggingFlags.OutputLevel,
                                                                        JetFitterInstance = "JetFitterTagFlip",
                                                                        usePtCorrectedMass = True,
                                                                        revertFromPositiveToNegativeTags = True)	
ToolSvc += NewJetFitterVariablesFactoryFlip
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterVariablesFactoryFlip

if (not BTaggingFlags.JetFitterTag):
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
NewJetFitterFlipTagTool = Analysis__JetFitterTag( name = "NewJetFitterTagFlip",
                                              OutputLevel = BTaggingFlags.OutputLevel,
                                              Runmodus = BTaggingFlags.Runmodus,
                                              JetFitterNtupleWriter = NewJetFitterNtupleWriter,
                                              JetFitterLikelihood = NewJetFitterLikelihoodFlip,
                                              jetCollectionList = BTaggingFlags.Jets,
                                              JetFitterVariablesFactory = NewJetFitterVariablesFactoryFlip,
                                              SecVxFinderName = "NewJetFitterVxFinderFlip",
                                              useForcedCalibration = False
                                              )

ToolSvc += NewJetFitterFlipTagTool


if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterFlipTagTool
