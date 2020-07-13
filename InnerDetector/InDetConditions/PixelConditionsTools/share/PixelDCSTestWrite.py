import AthenaCommon.AtlasUnixStandardJob

MyOutPut = INFO
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr


## from GaudiSvc.GaudiSvcConf import AuditorSvc

## ServiceMgr += AuditorSvc()
## theAuditorSvc = ServiceMgr.AuditorSvc
## theAuditorSvc.Auditors  += [ "ChronoAuditor"]
## #ChronoStatSvc = Service ( "ChronoStatSvc")
## theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
## #MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
## theApp.AuditAlgorithms=True

from AthenaCommon.GlobalFlags import GlobalFlags
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
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()


#include ("AtlasGeoModel/SetGeometryVersion.py")
import AtlasGeoModel.SetGeometryVersion
#include ("AtlasGeoModel/GeoModelInit.py")
import AtlasGeoModel.GeoModelInit


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc


#include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
import AthenaPoolCnvSvc.AthenaPool

from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr += PoolSvc( DbOpen = "update" )


from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
CondStreamPixelDCSTest = AthenaOutputStreamTool("CondStreamPixelDCSTest",OutputFile = "SimplePoolFile.root",OutputLevel = DEBUG)

ToolSvc += CondStreamPixelDCSTest

from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSTool

ToolSvc += PixelDCSTool(
                            OutputLevel = MyOutPut,
                            RegisterCallback = False,
                            TemperatureFolder = "/PIXEL/DCS/TEMPERATURE",
                            HVFolder = "/PIXEL/DCS/HV",
                            FSMStatusFolder = "/PIXEL/DCS/FSMSTATUS",
                            FSMStateFolder = "/PIXEL/DCS/FSMSTATE",
                            WriteDefault = FALSE,
                            Temperature = TRUE,
                            HV = TRUE,
                            FSMStatus = TRUE,
                            FSMState = TRUE,
                            TemperatureFile="TemperatureFile",
                            HVFile = "HVFile",
                            FSMStatusFile = "FSMStatusFile",
                            FSMStateFile = "FSMStateFile",
                            TemperatureFieldName = "temperature",
                            HVFieldName = "HV",
                            FSMStatusFieldName = "FSM_status",
                            FSMStateFieldName = "FSM_state",
                            TemperatureTag ="PixDCSTemperature-00-00-00", #"PixDCSTemperature-00-00-00",
                            HVTag = "PixDCSHV-00-00-00",#"PixDCSHV-00-00-00",
                            FSMStatusTag ="PixDCSFSMStatus-00-00-00", #"PixDCSFSMStatus-00-00-00",
                            FSMStateTag = "PixDCSFSMState-00-00-00",#"PixDCSFSMState-00-00-00",
                            TemperatureValue = -10,
                            HVValue = 150,
                            FSMStatusValue = "OK",
                            FSMStateValue = "READY"
                            )




from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSTestReadWrite

job += PixelDCSTestReadWrite(OutputLevel = MyOutPut, Write = True)


from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#MYIOVDbSvc = IOVDbSvc(dbConnection = "impl=cool;techno=sqlite;schema=mytest.db;X:TESTCOOL", GlobalTag = "DEFAULTCOND")
#MYIOVDbSvc = IOVDbSvc(dbConnection = "impl=cool;techno=sqlite;schema=mytest.db;X:TESTCOOL")
#MYIOVDbSvc = IOVDbSvc(dbConnection = "sqlite://X;schema=mytest.db;dbname=TESTCOOL")


#MYIOVDbSvc = IOVDbSvc(dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_AAD;DEVDB10:DCSDEC:ATLAS_COOL_AAD")
#MYIOVDbSvc = IOVDbSvc(dbConnection = "oracle://DEVDB10;schema=ATLAS_COOL_AAD;dbname=DCSTEST4")
#MYIOVDbSvc = IOVDbSvc(dbConnection = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_DCS;dbname=OFLP200") # atlas simulation
#MYIOVDbSvc = IOVDbSvc(dbConnection = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_DCS;dbname=COMP200")  # cosmics
MYIOVDbSvc = IOVDbSvc(dbConnection = "sqlite://X;schema=mytest.db;dbname=OFLP200")
IOVDbSvc.GlobalTag = "DEFAULTCOND"
ServiceMgr += MYIOVDbSvc


#theApp.Dlls += [ "RegistrationServices" ]
## include ( "RegistrationServices/RegistrationServices_jobOptions.py" )

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
MYIOVRegistrationSvc = IOVRegistrationSvc(IOVDbTimeStamp = TRUE)
MYIOVRegistrationSvc.OverrideNames+=['FSM_state','FSM_status']
MYIOVRegistrationSvc.OverrideTypes+=['String255','String255'] 
ServiceMgr += MYIOVRegistrationSvc




## include ( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )


import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.RunNumber         = 0
svcMgr.EventSelector.EventsPerRun      = 20
svcMgr.EventSelector.FirstEvent        = 1
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 10
svcMgr.EventSelector.FirstLB           = 0
svcMgr.EventSelector.EventsPerLB       = 2


# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = 3
svcMgr.MessageSvc.debugLimit       = 10000
svcMgr.MessageSvc.infoLimit        = 10000
svcMgr.MessageSvc.errorLimit       = 1000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"



theApp.EvtMax                   = 1

########################################### note #############################################
### to write the cosmics data in a single version folders this change was made in /Database/RegistrationServices/src/IOVRegistrationSvc.cxx


## // create folder
## 	    cool::FolderSpecification folderSpec(
## 						 // cool::FolderVersioning::MULTI_VERSION,payloadSpec);
## 						 cool::FolderVersioning::SINGLE_VERSION,payloadSpec);
## 	    folder = db->createFolder(local_folder,folderSpec,
##                   mergedNames,true); 
## 	    log << MSG::DEBUG <<" creation of CondDBFolder " << 
##                    local_folder << " done" << endmsg;

#################################################################################################
