from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

#initialize the detector description
include("RecExCond/AllDet_detDescr.py")

from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags

muonByteStreamFlags.MdtDataType = "atlas"

#initialize correctly the old cabling service
from MDTcabling.MDTcablingConfig import *

topSequence=AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.EventSelector.InputCollections = ["testfile.pool.root"]

from MuonMDT_Cabling.MuonMDT_CablingConf import MdtTestCabling, MuonMDT_CablingAlg
topSequence += MuonMDT_CablingAlg()

topSequence += MdtTestCabling()
MessageSvc.OutputLevel = INFO

#dbConn="oracle://intr;schema=ATLAS_COOL_MDTDQ;dbname=MDT_DQA;user=ATLAS_COOL_MDTDQ"
dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_MDT;dbname=COMP200;user=ATLAS_COOLONL_MDT"
folder="/MDT/CABLING/MAP_SCHEMA"
folder2="/MDT/CABLING/MEZZANINE_SCHEMA"
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=[folder2+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"] 

#configure the chrono service
ServiceMgr.ChronoStatSvc.PrintUserTime     = True
ServiceMgr.ChronoStatSvc.PrintSystemTime   = True
ServiceMgr.ChronoStatSvc.PrintEllapsedTime = True 

theApp.EvtMax = 10
