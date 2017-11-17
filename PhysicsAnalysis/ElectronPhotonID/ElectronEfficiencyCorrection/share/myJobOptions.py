#specify how many events it should process. Set it equal to -1 to process all events
theApp.EvtMax = 15
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections=[os.environ['ASG_TEST_FILE_MC']]   #insert your list of input files here


#One of the existing master sequences where one should attach all algorithms
from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()
# Add the test algorithm. the alg is a part of the tool, so it should be imported:
from ElectronEfficiencyCorrection.ElectronEfficiencyCorrectionConf import testAthenaElectronAlg
alg = testAthenaElectronAlg()

#define input files, for now it uses my own directory where the tool is
input_files=["ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v1/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v11.2015_2016.13TeV.rel20.7.25ns.v01.root"]
alg.ElectronEfficiencyCorrectionTool.CorrectionFileNameList = input_files;
#Set DataType: for data use 0 (or don't run the tool - faster), for FULLSIM use 1, and for FASTSIM use 3, please note that the input files are also should be different
alg.ElectronEfficiencyCorrectionTool.ForceDataType = 1
alg.ElectronEfficiencyCorrectionTool.CorrelationModel="MCTOYS"
alg.ElectronEfficiencyCorrectionTool.UseRandomRunNumber= False
alg.ElectronEfficiencyCorrectionTool.OutputLevel = INFO
alg.OutputLevel = DEBUG
algSeq += alg

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStream = MSMgr.NewPoolRootStream( "MyXAODStream", "myXAOD.pool.root" )
outStream.AddItem(['xAOD::ElectronContainer#MyElectrons'])
outStream.AddItem(['xAOD::ElectronAuxContainer#MyElectronsAux.'])
