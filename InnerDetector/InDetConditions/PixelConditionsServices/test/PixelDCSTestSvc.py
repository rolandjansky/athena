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


from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
# --- set defaults
GlobalFlags.DataSource.set_geant4()    
#GlobalFlags.InputFormat.set_pool()    
# --- default is zero luminosity
GlobalFlags.Luminosity.set_zero()
#GlobalFlags.DetDescrVersion = "ATLAS-CSC-02-00-00"
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


#DetDescrVersion = "ATLAS-DC3-05"

#include ("AtlasGeoModel/SetGeometryVersion.py")
import AtlasGeoModel.SetGeometryVersion
#include ("AtlasGeoModel/GeoModelInit.py")
import AtlasGeoModel.GeoModelInit



from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

include("PixelConditionsServices/SpecialPixelMapSvc_jobOptions.py")
include("PixelConditionsServices/PixelDCSSvc_jobOptions.py")
#include("PixelDCSSvc_jobOptions.py")

from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
ServiceMgr += PixelConditionsSummarySvc( IsActiveStatus=["OK", "WARNING"] ,  IsActiveStates= ["READY","ON"])


from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc.GlobalTag = "OFLCOND-FDR-02-01-00"

from PixelConditionsServices.PixelConditionsServicesConf import PixelDCSTestSvc

job += PixelDCSTestSvc()

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.RunNumber         = 91060
svcMgr.EventSelector.EventsPerRun      = 20
svcMgr.EventSelector.FirstEvent        = 1
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 10000
svcMgr.EventSelector.FirstLB           = 0
svcMgr.EventSelector.EventsPerLB       = 2


# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = 2
svcMgr.MessageSvc.debugLimit       = 10000
svcMgr.MessageSvc.infoLimit        = 10000
svcMgr.MessageSvc.errorLimit       = 1000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"



theApp.EvtMax                   = 2


conddb.blockFolder('/Indet/Align')
