from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc

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


#import AthenaPoolCnvSvc.ReadAthenaPool

#ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica2/data09_cos.00134804.physics_CosmicMuons.merge.AOD.f153_m187/data09_cos.00134804.physics_CosmicMuons.merge.AOD.f153_m187._0001.1"]
#ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica2/data09_cos.00136379.physics_IDCosmic.merge.AOD.f160_m213/data09_cos.00136379.physics_IDCosmic.merge.AOD.f160_m213._0001.1"]




#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
IOVDbSvc.OutputLevel = 3
IOVDbSvc.DBInstance="COOLOFL_DCS"
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

##-----------------------------------------------------------------
## MDT
#-----------------------------------------------------------------
# Dead Chambers

from MuonCondSvc.MuonCondSvcConf import MDT_DCSConditionsSvc
ServiceMgr +=MDT_DCSConditionsSvc()

from MuonCondTool.MuonCondToolConf import MDT_DCSConditionsTool
MDT_DCSConditionsTool = MDT_DCSConditionsTool("MDT_DCSConditionsTool")
MDT_DCSConditionsTool.OutputLevel = VERBOSE 
#MessageSvc.OutputLevel = DEBUG

MDT_DCSConditionsTool.DropChamberFolder = "/MDT/DCS/DROPPEDCH"
MDT_DCSConditionsTool.HVFolder = "/MDT/DCS/PSHVMLSTATE"
MDT_DCSConditionsTool.LVFolder = "/MDT/DCS/PSLVCHSTATE"
MDT_DCSConditionsTool.JTAGFolder = "/MDT/DCS/JTAGCHSTATE"

ToolSvc += MDT_DCSConditionsTool

dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
folder="/MDT/DCS/DROPPEDCH"


ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSHVMLSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSLVCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/JTAGCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]



IOVDbSvc.DBInstance="ATLAS_COOL_MDTDQ"
dbConn_Mdt="oracle://intr;schema=ATLAS_COOL_MDTDQ;dbname=MDT_DQA"
folder_Mdt="/CONFIG/DEAD_TUBE"

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn_Mdt
ServiceMgr.IOVDbSvc.Folders+=[folder_Mdt+" <tag>HEAD</tag> <dbConnection>"+dbConn_Mdt+"</dbConnection>"]




from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
ServiceMgr +=MDTCondSummarySvc()


MDTCondSummary=ServiceMgr.MDTCondSummarySvc
MDTCondSummary.ConditionsServices=["MDT_DCSConditionsSvc","MDT_DeadTubeConditionsSvc"]


from MuonCondTest.MuonCondTestConf import MDTConditionsTestAlg
job+= MDTConditionsTestAlg()


import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
#import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                   =  2 

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE
