print "Let's try on my own .... "
# working in 20.1.3.3 on Bytestream

#-- better than x306 (March 18, 2015)
#--conditionsTag 'CONDBR2-BLKPA-2015-02' \
#--geometryVersion 'ATLAS-R2-2015-03-00-00' \

if not 'EvtMax' in dir():
    EvtMax=100

if not 'ConditionsTag' in dir():
    ConditionsTag="CONDBR2-BLKPA-2015-02"

if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-R2-2015-03-00-00"

from AthenaCommon.AppMgr import theApp
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon import CfgGetter
from AthenaCommon.CfgGetter import getPublicTool, getPrivateTool, getService, getAlgorithm

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'data'          # or set this to 'geant4' if running on MC! ...
globalflags.DatabaseInstance = 'CONDBR2' # e.g. if you want run 2 data, set to COMP200 if you want run1

from IOVDbSvc.CondDB import conddb
import IOVDbSvc.IOVDb
ServiceMgr.IOVDbSvc.GlobalTag=ConditionsTag

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AthenaCommon.DetFlags import DetFlags; 
DetFlags.detdescr.ID_setOff();
DetFlags.detdescr.LAr_setOff();
DetFlags.detdescr.Tile_setOff();
DetFlags.detdescr.Muon_setOn();
DetFlags.readRDOBS.RPC_setOn(); 
from AtlasGeoModel import SetGeometryVersion;
from AtlasGeoModel import GeoModelInit

import MuonCnvExample.MuonCablingConfig

import MuonRPC_Cabling.MuonRPC_CablingConfig
#####################
#ServiceMgr.MuonRPC_CablingSvc.RPCMapfromCool=False
#ServiceMgr.MuonRPC_CablingSvc.ConfFileName="/afs/cern.ch/user/s/stefspa/atlas/SW/rpcDev/mapsFromGabriele/LVL1confAtlasRUN2_ver012.data"
#ServiceMgr.MuonRPC_CablingSvc.CorrFileName="/afs/cern.ch/user/s/stefspa/atlas/SW/rpcDev/mapsFromGabriele/LVL1confAtlasRUN2_ver012.corr"
#######################
#######################
#ServiceMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool=False
#######################
#######################

from MuonRPC_Cabling.MuonRPC_CablingConf import MuonRpcCablingTest
testAlgo = MuonRpcCablingTest()
testAlgo.selectedSide = ""
#testAlgo.selectedSide = "" ##### 21, 22 (25,26) for sector 12(14) side C ### 53,54 e 57,58 on side A 
#testAlgo.selectedSectors  = [ 21, 22, 53, 54 ]
#############################----sect 12----|---sect 13-----|----sect 14----##
testAlgo.selectedSectors  = [ 21, 22, 53, 54, 23, 24, 55, 56, 25, 26, 57, 58 ]
#testAlgo.selectedRois  = [ 0, 1 ]
#testAlgo.OutputLevel = INFO
#testAlgo.testOnlyFirstLastCMChannel = True
topSequence += testAlgo


######################## RPC BS to RDO offline algorithms 
include("MuonCnvExample/MuonReadBS_jobOptions.py")

######################## RPC rdo to prd 

from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
RpcRdoToPrepDataTool = Muon__RpcRdoToPrepDataTool("RpcPrepDataProviderTool")
#RpcRdoToPrepDataTool.dumpOffToOnlineMapToFile = True
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


RpcPrdProviderTool=getPublicTool("RpcPrepDataProviderTool")
RpcPrdProviderTool.OutputLevel = INFO
#RpcPrdProviderTool.OutputLevel = DEBUG
#RpcPrdProviderTool.RPCInfoFromDb  = True
PrcRdoDec=getPublicTool("RpcROD_Decoder")
PrcRdoDec.OutputLevel = INFO
#PrcRdoDec.OutputLevel = VERBOSE


from MuonSimHitToPrdTest.MuonSimHitToPrdTestConf import  RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo = RPC_SimHitToPrdCBNTAlgo()
topSequence += RPC_SimHitToPrdCBNTAlgo
RPC_SimHitToPrdCBNTAlgo.OutputLevel=INFO
RPC_SimHitToPrdCBNTAlgo.doMCtruth=False
RPC_SimHitToPrdCBNTAlgo.doRPCSimHit=False
RPC_SimHitToPrdCBNTAlgo.doRPCDigit=False
RPC_SimHitToPrdCBNTAlgo.doRPCRDO=True
RPC_SimHitToPrdCBNTAlgo.doRPCPrep=True

theApp.HistogramPersistency = "ROOT"
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.NTupleSvc.Output = [ "FILE DATAFILE='/tmp/stefspa/RPCntuple.root' OPT='NEW'" ]


ServiceMgr.MessageSvc.defaultLimit=99999999
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#MessageSvc.setDebug+=["RpcROD_Decoder"]

from ByteStreamCnvSvc import ReadByteStream
ServiceMgr.EventSelector.Input=["root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_cos/physics_CosmicMuons/00256385/data15_cos.00256385.physics_CosmicMuons.merge.RAW/data15_cos.00256385.physics_CosmicMuons.merge.RAW._lb0615._SFO-13._0001.1"]

#import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.EventSelector.InputBSCollections = [ "root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_cos/physics_CosmicMuons/00256385/data15_cos.00256385.physics_CosmicMuons.merge.RAW/data15_cos.00256385.physics_CosmicMuons.merge.RAW._lb0615._SFO-13._0001.1" ]

theApp.EvtMax = EvtMax

MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 99999999
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

print "out NtupleFragment"


