useNNcombinedWithIP3D=False

from AthenaCommon.AppMgr import ToolSvc

if not BTaggingFlags.JetFitterNN:
  from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
  JetFitterNtupleWriterNN = Analysis__JetFitterNtupleWriter( name = "JetFitterNtupleWriterNN",
                                                             OutputLevel = BTaggingFlags.OutputLevel)

  ToolSvc += JetFitterNtupleWriterNN
  if BTaggingFlags.OutputLevel < 3:
    print JetFitterNtupleWriterNN   

from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
JetFitterVariablesFactoryNNFlip = Analysis__JetFitterVariablesFactory( name = "JetFitterVariablesFactoryNNFlip",
                                                                       OutputLevel = BTaggingFlags.OutputLevel,
                                                                       secVxFinderName = "NewJetFitterVxFinderFlip",
                                                                       JetFitterInstance = "JetFitterTagNNFlip",
                                                                       usePtCorrectedMass = True,
                                                                       revertFromPositiveToNegativeTags = True)	
ToolSvc += JetFitterVariablesFactoryNNFlip
if BTaggingFlags.OutputLevel < 3:
  print JetFitterVariablesFactoryNNFlip


if not BTaggingFlags.JetFitterNN:
  from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
  NeuralNetworkToHistoNNTool = Trk__NeuralNetworkToHistoTool ( name = "NeuralNetworkToHistoToolNN",
                                                               OutputLevel = BTaggingFlags.OutputLevel)
                                                                      
  ToolSvc += NeuralNetworkToHistoNNTool
  if BTaggingFlags.OutputLevel < 3:
    print NeuralNetworkToHistoNNTool


  from JetTagTools.JetTagToolsConf import Analysis__JetFitterNNTool
  JetFitterNNTool = Analysis__JetFitterNNTool( name = "JetFitterNNTool",
                                               CalibrationDirectory="JetFitter",
                                               CalibrationSubDirectory="NeuralNetwork",
                                               OutputLevel = BTaggingFlags.OutputLevel,
                                               NeuralNetworkToHistoTool = NeuralNetworkToHistoNNTool,
                                               useCombinedIPNN = useNNcombinedWithIP3D,
                                               calibrationTool=BTagCalibrationBrokerTool
                                               )
  ToolSvc += JetFitterNNTool
  if BTaggingFlags.OutputLevel < 3:
    print JetFitterNNTool

from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
JetFitterFlipTagToolNN = Analysis__JetFitterTag( name = "JetFitterTagNNFlip",
                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                 Runmodus = BTaggingFlags.Runmodus,
                                                 jetfitterNtupleWriter = JetFitterNtupleWriterNN,
                                                 jetfitterClassifier = JetFitterNNTool,
                                                 jetfitterVariablesFactory = JetFitterVariablesFactoryNNFlip,
                                                 jetCollectionList = BTaggingFlags.Jets,
                                                 useForcedCalibration = False,
                                                 supplementalTaggers = ['IP3D'] if useNNcombinedWithIP3D else [],
                                                 storeOnlyBaseObject = True
                                                 )

if not BTaggingFlags.JetFitterTagFlip:
  JetFitterFlipTagToolNN.storeOnlyBaseObject = False

ToolSvc += JetFitterFlipTagToolNN

if BTaggingFlags.OutputLevel < 3:
  print JetFitterFlipTagToolNN
