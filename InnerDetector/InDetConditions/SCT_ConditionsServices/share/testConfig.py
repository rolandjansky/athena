import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-GEO-16-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.detdescr.SCT_setOn()
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()
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

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3

#ToolSvc = ServiceMgr.ToolSvc
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Chip")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Module")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/MUR")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/ROD")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Geog")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/RODMUR")

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
MyConfigConditions = SCT_ConfigurationConditionsSvc("MyConfigConditions")
ServiceMgr += MyConfigConditions

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsTestAlg
MyConfigTestAlg = SCT_ConfigurationConditionsTestAlg("MyConfigTestAlg")
MyConfigTestAlg.ConfigConditions = MyConfigConditions
job += MyConfigTestAlg

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 96982 #90272 #40341 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  = 1228142560 #1222568868 #1204216576 #found valid in db browser?
theApp.EvtMax                              = 1

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
