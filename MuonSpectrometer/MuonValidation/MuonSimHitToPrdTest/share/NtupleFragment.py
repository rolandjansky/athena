
from AthenaCommon.AlgSequence import AlgSequence
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.OutputLevel = INFO
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.ValidationSetup = 1
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.BOG_BOF_DoubletR2_OFF = 0
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.turnON_efficiency = 0 # gaps and panels always efficient
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.Efficiency_fromCOOL = 0 # irrelevant since the above 
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.turnON_clustersize = 1 # gaps and panels always efficient
AlgSequence().RPC_Digitizer.RPC_DigitizationTool.ClusterSize_fromCOOL = 0 # irrelevant since the above 


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


######################## RPC rdo to prd 

from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
RpcRdoToPrepDataTool = Muon__RpcRdoToPrepDataTool("RpcPrepDataProviderTool")
# property had to be removed for technical reasons
# RpcRdoToPrepDataTool.dumpOffToOnlineMapToFile = False
ToolSvc += RpcRdoToPrepDataTool

from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
RpcRdoToRpcPrepData = RpcRdoToRpcPrepData()
RpcRdoToRpcPrepData.PrintPrepData = False
from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
RpcRdoToRpcPrepData.RegSel_RPC = makeRegSelTool_RPC()

topSequence += RpcRdoToRpcPrepData
######################## end of RPC rdo to prd 


######################## CSC rdo to prd and clusterization 
#
#from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
#CscRdoToPrepDataTool = Muon__CscRdoToCscPrepDataTool("CscPrepDataProviderTool")
#ToolSvc += CscRdoToPrepDataTool
#
#from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
#CscRdoToCscPrepData = CscRdoToCscPrepData()
#CscRdoToCscPrepData.PrintPrepData = False
#
#topSequence += CscRdoToCscPrepData
#
#include("CscClusterization/CscThresholdClusterizationOptions.py") 
######################## end of CSC rdo to prd and clusterization 


from MuonSimHitToPrdTest.MuonSimHitToPrdTestConf import  RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo = RPC_SimHitToPrdCBNTAlgo()
topSequence += RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo.OutputLevel=DEBUG
RPC_SimHitToPrdCBNTAlgo.doRPCRDO=False
RPC_SimHitToPrdCBNTAlgo.doRPCPrep=False

theApp.HistogramPersistency = "ROOT"
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.NTupleSvc.Output = [ "FILE DATAFILE='RPCntuple.root' OPT='NEW'" ]

























