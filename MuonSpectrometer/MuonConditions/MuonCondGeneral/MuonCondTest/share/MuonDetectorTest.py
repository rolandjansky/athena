from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

topSequence=AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d98/14.3.0/MuonSpectrometer/MuonCablings/MuonMDT_Cabling/MuonMDT_Cabling-00-00-04/share/AOD.032240._00100.pool.root.1"]

from MuonCondTest.MuonCondTestConf import MuonDetectorStatusTest

topSequence += MuonDetectorStatusTest()

from MuonCondTool.MuonCondToolConf import MuonDetectorStatusDbTool
MuonDetectorStatusDbTool = MuonDetectorStatusDbTool("MuonDetectorStatusDbTool")
MuonDetectorStatusDbTool.OutputLevel = DEBUG 
#MessageSvc.OutputLevel = DEBUG

MuonDetectorStatusDbTool.TubeFolder = "/OFFLINE/DQMFCALSTREAM/DQMFCALSTREAM_DEAD"

ToolSvc += MuonDetectorStatusDbTool

dbConn="oracle://intr;schema=ATLAS_COOL_MDTDQ;dbname=MDT_DQA;user=ATLAS_COOL_MDTDQ"
folder="/OFFLINE/DQMFCALSTREAM/DQMFCALSTREAM_DEAD"

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]



theApp.EvtMax = 100
