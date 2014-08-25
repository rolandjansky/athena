from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

topSequence=AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d98/14.3.0/MuonSpectrometer/MuonCablings/MuonMDT_Cabling/MuonMDT_Cabling-00-00-04/share/AOD.032240._00100.pool.root.1"]

from MuonCondTest.MuonCondTestConf import RpcStatusTest

topSequence += RpcStatusTest()

from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
RpcDetectorStatusDbTool.OutputLevel = DEBUG 
#MessageSvc.OutputLevel = DEBUG

RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"

ToolSvc += RpcDetectorStatusDbTool

dbConn="oracle://intr;schema=ATLAS_COOL_RPCDQ;dbname=RPC_DQA"
#;user=ATLAS_COOL_RPCDQ"
folder="/OFFLINE/FINAL"

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>RecoCondDB</tag> <dbConnection>"+dbConn+"</dbConnection>"]



theApp.EvtMax = 10
