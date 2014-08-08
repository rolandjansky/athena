useNNcombinedWithIP3D=True

from AthenaCommon.AppMgr import ToolSvc

if not BTaggingFlags.JetFitterNN:
  from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
  JetFitterNtupleWriterCOMBNN = Analysis__JetFitterNtupleWriter( name = "JetFitterNtupleWriterCOMBNN",
                                                                 OutputLevel = BTaggingFlags.OutputLevel)

  ToolSvc += JetFitterNtupleWriterCOMBNN
  if BTaggingFlags.OutputLevel < 3:
    print JetFitterNtupleWriterCOMBNN   

from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
JetFitterVariablesFactoryCOMBNNIP3DNeg = Analysis__JetFitterVariablesFactory( name = "JetFitterVariablesFactoryCOMBNNIP3DNeg",
                                                                           OutputLevel = BTaggingFlags.OutputLevel,
                                                                           JetFitterInstance = "JetFitterCOMBNNIP3DNeg",
                                                                           secVxFinderName = "NewJetFitterVxFinderFlip",
                                                                           usePtCorrectedMass = True,
                                                                           revertFromPositiveToNegativeTags = True)	
ToolSvc += JetFitterVariablesFactoryCOMBNNIP3DNeg
if BTaggingFlags.OutputLevel < 3:
  print JetFitterVariablesFactoryCOMBNNIP3DNeg


if not BTaggingFlags.JetFitterNN:
  from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
  NeuralNetworkToHistoCOMBNNTool = Trk__NeuralNetworkToHistoTool ( name = "NeuralNetworkToHistoToolCOMBNN",
                                                                   OutputLevel = BTaggingFlags.OutputLevel)



  ToolSvc += NeuralNetworkToHistoCOMBNNTool
  if BTaggingFlags.OutputLevel < 3:
    print NeuralNetworkToHistoCOMBNNTool

  from JetTagTools.JetTagToolsConf import Analysis__JetFitterNNTool
  JetFitterCOMBNNTool = Analysis__JetFitterNNTool( name = "JetFitterCOMBNNTool",
                                                   CalibrationDirectory="JetFitter",
                                                   CalibrationSubDirectory="NeuralNetwork",
                                                   OutputLevel = BTaggingFlags.OutputLevel,
                                                   NeuralNetworkToHistoTool = NeuralNetworkToHistoNNTool,
                                                   useCombinedIPNN = useNNcombinedWithIP3D,
                                                   calibrationTool=BTagCalibrationBrokerTool
                                                   )
  ToolSvc += JetFitterCOMBNNTool
  if BTaggingFlags.OutputLevel < 3:
    print JetFitterCOMBNNTool

from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
JetFitterIP3DNegCOMBNN = Analysis__JetFitterTag( name = "JetFitterTagCOMBNNIP3DNeg",
                                          OutputLevel = BTaggingFlags.OutputLevel,
                                          Runmodus = BTaggingFlags.Runmodus,
                                          jetfitterNtupleWriter = JetFitterNtupleWriterCOMBNN,
                                          jetfitterClassifier = JetFitterCOMBNNTool,
                                          jetfitterVariablesFactory = JetFitterVariablesFactoryCOMBNNIP3DNeg,
                                          useForcedCalibration = False,
                                          supplementalTaggers = ["IP3DNeg"] if useNNcombinedWithIP3D else [],
                                          storeOnlyBaseObject = True
                                          )

ToolSvc += JetFitterIP3DNegCOMBNN


JetFitterIP3DNegCOMBNN.jetCollectionList = []
for key in BTaggingFlags.Jets:
    JetFitterIP3DNegCOMBNN.jetCollectionList += [key]
#    if (key == 'Cone4Tower' or key == 'Cone7Tower'):

if BTaggingFlags.OutputLevel < 3:
  print JetFitterIP3DNegCOMBNN
