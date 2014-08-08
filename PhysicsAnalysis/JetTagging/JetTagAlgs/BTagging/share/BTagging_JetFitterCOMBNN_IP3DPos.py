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
JetFitterVariablesFactoryCOMBNNIP3DPos = Analysis__JetFitterVariablesFactory( name = "JetFitterVariablesFactoryCOMBNNIP3DPos",
                                                                           OutputLevel = BTaggingFlags.OutputLevel,
                                                                           JetFitterInstance = "JetFitterCOMBNNIP3DPos",
                                                                           secVxFinderName = "NewJetFitterVxFinderFlip",
                                                                           usePtCorrectedMass = True,
                                                                           revertFromPositiveToNegativeTags = True)	
ToolSvc += JetFitterVariablesFactoryCOMBNNIP3DPos
if BTaggingFlags.OutputLevel < 3:
  print JetFitterVariablesFactoryCOMBNNIP3DPos


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
JetFitterIP3DPosCOMBNN = Analysis__JetFitterTag( name = "JetFitterTagCOMBNNIP3DPos",
                                          OutputLevel = BTaggingFlags.OutputLevel,
                                          Runmodus = BTaggingFlags.Runmodus,
                                          jetfitterNtupleWriter = JetFitterNtupleWriterCOMBNN,
                                          jetfitterClassifier = JetFitterCOMBNNTool,
                                          jetfitterVariablesFactory = JetFitterVariablesFactoryCOMBNNIP3DPos,
                                          useForcedCalibration = False,
                                          supplementalTaggers = ['IP3DPos'] if useNNcombinedWithIP3D else [],
                                          storeOnlyBaseObject = True
                                          )

ToolSvc += JetFitterIP3DPosCOMBNN


JetFitterIP3DPosCOMBNN.jetCollectionList = []
for key in BTaggingFlags.Jets:
    JetFitterIP3DPosCOMBNN.jetCollectionList += [key]
#    if (key == 'Cone4Tower' or key == 'Cone7Tower'):

if BTaggingFlags.OutputLevel < 3:
  print JetFitterIP3DPosCOMBNN
