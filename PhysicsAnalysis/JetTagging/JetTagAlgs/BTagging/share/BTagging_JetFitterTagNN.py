useNNcombinedWithIP3D=False
from AthenaCommon.AppMgr import ToolSvc

from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
JetFitterNtupleWriterNN = Analysis__JetFitterNtupleWriter( name = "JetFitterNtupleWriterNN",
                                                           OutputLevel = BTaggingFlags.OutputLevel)

ToolSvc += JetFitterNtupleWriterNN
if BTaggingFlags.OutputLevel < 3:
  print JetFitterNtupleWriterNN   


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
                                             usePtCorrectedMass = True,
					     calibrationTool=BTagCalibrationBrokerTool
                                             )
ToolSvc += JetFitterNNTool
if BTaggingFlags.OutputLevel < 3:
  print JetFitterNNTool

from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
JetFitterTagToolNN = Analysis__JetFitterTag( name = "JetFitterTagNN",
                                             OutputLevel = BTaggingFlags.OutputLevel,
                                             Runmodus = BTaggingFlags.Runmodus,
                                             jetfitterNtupleWriter = JetFitterNtupleWriterNN,
                                             jetfitterClassifier = JetFitterNNTool,
                                             jetCollectionList = BTaggingFlags.Jets,
                                             useForcedCalibration = False,
                                             ipinfoTaggerName = "IP3D" if useNNcombinedWithIP3D else "",
                                             SecVxFinderName="JetFitter",
                                             xAODBaseName="JetFitter"
                                             )





#if not BTaggingFlags.JetFitterTag:
#  JetFitterTagToolNN.storeOnlyBaseObject = False

ToolSvc += JetFitterTagToolNN

if BTaggingFlags.OutputLevel < 3:
  print JetFitterTagToolNN
