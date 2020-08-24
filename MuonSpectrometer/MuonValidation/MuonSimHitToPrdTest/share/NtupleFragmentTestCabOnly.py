
print "in NtupleFragment"
from AthenaCommon.AlgSequence import AlgSequence

from AthenaCommon import CfgGetter
from AthenaCommon.CfgGetter import getPublicTool, getPrivateTool, getService, getAlgorithm
########### in principle one shoudl just do: 
#RPC_DigitizationTool=getPublicTool("RpcDigitizationTool")
#RPC_DigitizationTool.ValidationSetup = 1   
########### for some reason (mistakes in https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/Digitization/trunk/python/DigiAlgConfig.py#L169) ?? 
########### it does not work. 
#### quick solution for rel 19 (pileup tool)
#job = AlgSequence()
#job.StandardPileUpToolsAlg.PileUpTools["RpcDigitizationTool"].ValidationSetup = 1
#### general (working in all releases) temporary solution 
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from Digitization.DigitizationFlags import digitizationFlags
rpcdigi = None
if not digitizationFlags.doXingByXingPileUp() and hasattr(job, 'RPC_Digitization'):
  rpcdigi = job.RPC_Digitization
else:
  for alg in job:
     if hasattr(alg, 'PileUpTools'):
        rpcdigi = alg.PileUpTools[ "RpcDigitizationTool" ]
        break
     pass
if None == rpcdigi:
  raise AttributeError("RpcDigitization(Tool) not found.")
rpcdigi.OutputLevel=DEBUG                                         
rpcdigi.OutputLevel = INFO                                        
rpcdigi.ValidationSetup = 1                                       
rpcdigi.BOG_BOF_DoubletR2_OFF = 1                                 
rpcdigi.turnON_efficiency = 0 # gaps and panels always efficient  
rpcdigi.Efficiency_fromCOOL = 0 # irrelevant since the above      
rpcdigi.turnON_clustersize = 1 # gaps and panels always efficient 
rpcdigi.ClusterSize_fromCOOL = 0 # irrelevant since the above     


from AthenaCommon.AppMgr import ServiceMgr
import MuonRPC_Cabling.MuonRPC_CablingConfig
#####################
ServiceMgr.MuonRPC_CablingSvc.RPCMapfromCool=True
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="ConfFileName_type7_12_16_17_v006.txt" # default is "ConfFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="ConfFileName_type7_v001.txt" # default is "ConfFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="ConfFileName_type7_v002.txt" # default is "ConfFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="ConfFileName_type7_v003.txt" # default is "ConfFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="ConfFileName_type7_v004.txt" # default is "ConfFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="ConfFileName_type7.txt" # default is "ConfFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.CorrFileName="CorrFileName.txt"
#ServiceMgr.MuonRPC_CablingSvc.CorrFileName="CorrFileName_v003.txt"
#ServiceMgr.MuonRPC_CablingSvc.CorrFileName="CorrFileName_v004.txt"
#ServiceMgr.MuonRPC_CablingSvc.CorrFileName="CorrFileName_v007.txt"
#######################
#######################
ServiceMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool=True
#ServiceMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool=False
#ServiceMgr.MuonRPC_CablingSvc.DatabaseRepository="DUMMY";
#######################
#######################


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from MuonRPC_Cabling.MuonRPC_CablingConf import MuonRpcCablingTest
testAlgo = MuonRpcCablingTest()
#testAlgo.selectedSide = "A"
testAlgo.selectedSide = "" ##### 21, 22 (25,26) for sector 12(14) side C ### 53,54 e 57,58 on side A 
#testAlgo.selectedSectors  = [ 21, 22, 53, 54 ]
testAlgo.selectedSectors  = [ 21 ]
#testAlgo.selectedRois  = [ 0, 1 ]
#testAlgo.OutputLevel = INFO
#testAlgo.testOnlyFirstLastCMChannel = True
topSequence += testAlgo


######################## RPC rdo to prd 

from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
RpcRdoToPrepDataTool = Muon__RpcRdoToPrepDataTool("RpcPrepDataProviderTool")
# property had to be removed for technical reasons
# RpcRdoToPrepDataTool.dumpOffToOnlineMapToFile = False
ToolSvc += RpcRdoToPrepDataTool
#
from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
RpcRdoToRpcPrepData = RpcRdoToRpcPrepData()
RpcRdoToRpcPrepData.PrintPrepData = False
from RegionSelector.RegSelToolConfig import makeRegSelTool_RPC
RpcRdoToRpcPrepData.RegSel_RPC = makeRegSelTool_RPC()
#
topSequence += RpcRdoToRpcPrepData
######################## end of RPC rdo to prd 


from MuonSimHitToPrdTest.MuonSimHitToPrdTestConf import  RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo = RPC_SimHitToPrdCBNTAlgo()
topSequence += RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo.OutputLevel=INFO
RPC_SimHitToPrdCBNTAlgo.doRPCRDO=True
RPC_SimHitToPrdCBNTAlgo.doRPCPrep=True

theApp.HistogramPersistency = "ROOT"
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.NTupleSvc.Output = [ "FILE DATAFILE='RPCntuple.root' OPT='NEW'" ]



ServiceMgr.MessageSvc.defaultLimit=99999999
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

print "out NtupleFragment"




















