useNNcombinedWithIP3D=True

from AthenaCommon.AppMgr import ToolSvc

from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
JetFitterNtupleWriterCOMBNN = Analysis__JetFitterNtupleWriter( name = "JetFitterNtupleWriterCOMBNN",
                                                           OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += JetFitterNtupleWriterCOMBNN
if BTaggingFlags.OutputLevel < 3:
  print JetFitterNtupleWriterCOMBNN   



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
                                                 usePtCorrectedMass = True,
                                                 useCombinedIPNN = useNNcombinedWithIP3D,
					         calibrationTool=BTagCalibrationBrokerTool
                                               )
ToolSvc += JetFitterCOMBNNTool
if BTaggingFlags.OutputLevel < 3:
  print JetFitterCOMBNNTool

from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
JetFitterCOMBNN = Analysis__JetFitterTag( name = "JetFitterTagCOMBNN",
                                          OutputLevel = BTaggingFlags.OutputLevel,
                                          Runmodus = BTaggingFlags.Runmodus,
                                          jetfitterNtupleWriter = JetFitterNtupleWriterCOMBNN,
                                          jetfitterClassifier = JetFitterCOMBNNTool,
                                          useForcedCalibration = False,
                                          ipinfoTaggerName = "IP3D" if useNNcombinedWithIP3D else "",
                                          SecVxFinderName="JetFitter",
                                          xAODBaseName="JetFitterCombNN"
                                          )

ToolSvc += JetFitterCOMBNN


JetFitterCOMBNN.jetCollectionList = []
for key in BTaggingFlags.Jets:
    JetFitterCOMBNN.jetCollectionList += [key]
#    if (key == 'Cone4Tower' or key == 'Cone7Tower'):

if BTaggingFlags.OutputLevel < 3:
  print JetFitterCOMBNN
