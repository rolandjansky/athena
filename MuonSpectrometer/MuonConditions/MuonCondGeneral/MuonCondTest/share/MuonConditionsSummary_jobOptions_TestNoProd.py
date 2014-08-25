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

GlobalFlags.DetGeo.set_Value_and_Lock()
#from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
#GlobalFlags.DetGeo.set_atlas()
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


import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica/data10_7TeV.00158116.physics_MinBias.merge.AOD.f271_m533/data10_7TeV.00158116.physics_MinBias.merge.AOD.f271_m533._lb0002-lb0042.1"]
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
ServiceMgr +=MDT_DCSConditionsSvc()out

from MuonCondTool.MuonCondToolConf import MDT_DCSConditionsTool
MDT_DCSConditionsTool = MDT_DCSConditionsTool("MDT_DCSConditionsTool")
MDT_DCSConditionsTool.OutputLevel = VERBOSE 
#MessageSvc.OutputLevel = DEBUG

MDT_DCSConditionsTool.DropChamberFolder = "/MDT/DCS/DROPPEDCH"
MDT_DCSConditionsTool.HVFolder = "/MDT/DCS/PSHVMLSTATE"
MDT_DCSConditionsTool.LVFolder = "/MDT/DCS/PSLVCHSTATE"
MDT_DCSConditionsTool.JTAGFolder = "/MDT/DCS/JTAGCHSTATE"
MDT_DCSConditionsTool.SetPointsV0Folder ="/MDT/DCS/PSV0SETPOINTS"
MDT_DCSConditionsTool.SetPointsV1Folder = "/MDT/DCS/PSV1SETPOINTS"

ToolSvc += MDT_DCSConditionsTool

dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
#folder="/MDT/DCS/DROPPEDCH"
#dbConn="sqlite://;schema=MDT_CHAMBER.db;dbname=OFLP200"

ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/DROPPEDCH"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSHVMLSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSLVCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/JTAGCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSV0SETPOINTS"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSV1SETPOINTS"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]



from MuonCondSvc.MuonCondSvcConf import MDT_DeadTubeConditionsSvc
ServiceMgr +=MDT_DeadTubeConditionsSvc()

from MuonCondTool.MuonCondToolConf import MDT_DeadTubeConditionsTool
MDT_DeadTubeConditionsTool = MDT_DeadTubeConditionsTool("MDT_DeadTubeConditionsTool")
MDT_DeadTubeConditionsTool.OutputLevel = VERBOSE 
#MessageSvc.OutputLevel = DEBUG
MDT_DeadTubeConditionsTool.DeadTubeFolder = "/MDT/TUBE_STATUS/DEAD_TUBE"
ToolSvc += MDT_DeadTubeConditionsTool

#dbConn_Mdt="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_MDT;dbname=OFLP200"
dbConn_Mdt="sqlite://;schema=MDT_DEAD_TUBE_OFL.db;dbname=OFLP200"

ServiceMgr.IOVDbSvc.dbConnection=dbConn_Mdt
ServiceMgr.IOVDbSvc.Folders+=["/MDT/TUBE_STATUS/DEAD_TUBE"+" <tag>HEAD</tag> <dbConnection>"+dbConn_Mdt+"</dbConnection>"]




from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
ServiceMgr +=MDTCondSummarySvc()


MDTCondSummary=ServiceMgr.MDTCondSummarySvc
MDTCondSummary.ConditionsServices=["MDT_DCSConditionsSvc","MDT_DeadTubeConditionsSvc"]






## ##-----------------------------------------------------------------------------
## ### TGC Staff

## from MuonCondSvc.MuonCondSvcConf import TGC_STATUSConditionsSvc
## ServiceMgr +=TGC_STATUSConditionsSvc()

## from MuonCondTool.MuonCondToolConf import TGC_STATUSConditionsTool
## TGC_STATUSConditionsTool = TGC_STATUSConditionsTool("TGC_STATUSConditionsTool")
## TGC_STATUSConditionsTool.OutputLevel = DEBUG 
## MessageSvc.OutputLevel = DEBUG

## TGC_STATUSConditionsTool.TgcDqFolder = "/TGC/1/DetectorStatus"

## ToolSvc += TGC_STATUSConditionsTool

## folderTGC="/TGC/1/DetectorStatus"
## dbConnTGC="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TGC;dbname=COMP200"#;user=ATLAS_COOL_READER_U"

## ServiceMgr.IOVDbSvc.dbConnection=dbConnTGC
## #ServiceMgr.IOVDbSvc.DBInstance="COOLONL_TGC"
## ServiceMgr.IOVDbSvc.Folders+=[folderTGC+" <tag>Tgc1DetectorStatus-V1-0</tag> <dbConnection>"+dbConnTGC+"</dbConnection>"]

## from MuonCondSvc.MuonCondSvcConf import TGCCondSummarySvc
## ServiceMgr +=TGCCondSummarySvc()


## TGCCondSummary=ServiceMgr.TGCCondSummarySvc
## TGCCondSummary.ConditionsServices=["TGC_STATUSConditionsSvc"]




from MuonCondTest.MuonCondTestConf import MuonConditionsTestAlg
job+= MuonConditionsTestAlg()


#import AthenaCommon.AtlasUnixGeneratorJob

#ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
#import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
#ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                   =  2 

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE
