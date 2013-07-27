import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

## general conf

from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DetGeo.set_commis()


from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()


from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.AtlasVersion = "ATLAS-GEO-01-00-00"

#specific algos/tools

### write here the db connection string

# access to db directly
#dbConn="oracle://devdb10;schema=ATLAS_RPC;dbname=RPC_DQA;user=ATLAS_RPC"
# use sqlite file
dbConn="sqlite://;schema=RPCONLINE2.db;dbname=RPC_DQA"

### write here the folder to use

folder='/ONLINE/ONLINE_MASK'


from MuonCondSvc.MuonCondSvcConf import MuonCalib__RpcCoolStrSvc

RpcCoolStrSvc=MuonCalib__RpcCoolStrSvc("RpcCoolStrSvc")
RpcCoolStrSvc.OutputLevel=DEBUG
RpcCoolStrSvc.Folder=folder

#from IOVDbSvc.IOVDbSvcConf import IOVDbSvc

from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn

ServiceMgr += RpcCoolStrSvc

# RpcCoolStrSvc

from MuonCondCool.MuonCondCoolConf import MuonCalib__RpcReadWriteCoolStr

RpcReadWriteCoolStr=MuonCalib__RpcReadWriteCoolStr()

RpcReadWriteCoolStr.OutputLevel=DEBUG
RpcReadWriteCoolStr.Write=False
RpcReadWriteCoolStr.Read=False
RpcReadWriteCoolStr.WriteToOnlineDb=False
RpcReadWriteCoolStr.ReadFromOnlineDb=True
RpcReadWriteCoolStr.FileName="testVincenzo55.txt"

# data is written to conditions database using OutputConditionsAlg

if(RpcReadWriteCoolStr.Write or RpcReadWriteCoolStr.WriteToOnlineDb):
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")
    
    prefix = "CondAttrListCollection#"
    OutCondAlg.ObjectList=[prefix+folder]
    OutCondAlg.IOVTagList = [folder+" <tag>HEAD</tag>"]
    OutCondAlg.WriteIOV=True
    OutCondAlg.Run1=100
    #OutputConditionsAlg.Event1=0
    OutCondAlg.Run2=1000
    #OutputConditionsAlg.Event2=9999
    OutCondAlg.OutputLevel=DEBUG
else:
    ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]

job+=RpcReadWriteCoolStr

print ServiceMgr

theApp.EvtMax=1

#theApp.initialize()
