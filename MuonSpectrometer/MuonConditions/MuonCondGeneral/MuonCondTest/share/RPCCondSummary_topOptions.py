from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True


#from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
# --- set defaults
GlobalFlags.DataSource.set_geant4()   
GlobalFlags.InputFormat.set_pool()   
# --- default is zero luminosity
GlobalFlags.Luminosity.set_zero()
GlobalFlags.Print()

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.SCT_setOff()
#DetFlags.ID_setOn()
#DetFlags.Calo_setOff()
#DetFlags.Muon_setOn()
#DetFlags.Truth_setOff()
#DetFlags.LVL1_setOff()
#DetFlags.SCT_setOn()
#DetFlags.TRT_setOff()

# ---- switch parts of ID off/on as follows
#switch off tasks
#DetFlags.pileup.all_setOff()
#DetFlags.simulate.all_setOff()
#DetFlags.makeRIO.all_setOff()
#DetFlags.writeBS.all_setOff()
#DetFlags.readRDOBS.all_setOff()

#DetFlags.readRIOBS.all_setOff()
#DetFlags.readRIOPool.all_setOff()
#DetFlags.writeRIOPool.all_setOff()

#import AtlasGeoModel.SetGeometryVersion
#import AtlasGeoModel.GeoModelInit

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOff()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

#import AtlasGeoModel.SetGeometryVersion
#import AtlasGeoModel.GeoModelInit

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc

#DetDescrVersion="ATLAS-CSC-01-02-00"
from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = DetDescrVersion

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True
DetDescrVersion = "ATLAS-GEO-08-00-02"
jobproperties.Global.DetDescrVersion = DetDescrVersion
#DetDescrVersion = GlobalFlags.DetDescrVersion()
GeoModelSvc.AtlasVersion = DetDescrVersion


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
#IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3

# Dead Chambers

from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
ServiceMgr +=RPC_STATUSConditionsSvc()


from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
RpcDetectorStatusDbTool.OutputLevel = DEBUG 
#MessageSvc.OutputLevel = DEBUG

RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"

ToolSvc += RpcDetectorStatusDbTool

dbConn="oracle://intr;schema=ATLAS_COOL_RPCDQ;dbname=RPC_DQA"
#;user=ATLAS_COOL_RPCDQ"
folder="/OFFLINE/FINAL"

include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>RecoCondDB</tag> <dbConnection>"+dbConn+"</dbConnection>"]




from MuonCondSvc.MuonCondSvcConf import RPCCondSummarySvc
ServiceMgr +=RPCCondSummarySvc()


RPCCondSummary=ServiceMgr.RPCCondSummarySvc
RPCCondSummary.ConditionsServices=["RPC_STATUSConditionsSvc"]


from MuonCondTest.MuonCondTestConf import RPCStatusTestAlg
job+= RPCStatusTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                   =  2 

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE
