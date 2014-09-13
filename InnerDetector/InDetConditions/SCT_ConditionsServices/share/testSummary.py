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


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-GEO-16-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags




#from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
#GlobalFlags.DetGeo.set_atlas()
#globalflags.DetGeo.set_Value_and_Lock(blah)
# --- set defaults
#GlobalFlags.DataSource.set_geant4()    
#GlobalFlags.InputFormat.set_pool()    
# --- default is zero luminosity
#GlobalFlags.Luminosity.set_zero()
#GlobalFlags.Print()

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
print "conddb.dbdata", conddb.dbdata
IOVDbSvc.OutputLevel = 3

#ToolSvc = ServiceMgr.ToolSvc

conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Chip")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Module")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/MUR")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/ROD")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Geog")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/RODMUR")
conddb.addFolder('',"<db>COOLONL_TDAQ/COMP200</db> /TDAQ/EnabledResources/ATLAS/SCT/Robins")






from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoSvc
ServiceMgr +=SCT_ModuleVetoSvc()
SCT_ModuleVeto=ServiceMgr.SCT_ModuleVetoSvc
SCT_ModuleVeto.BadModuleIdentifiers=["1","2"]

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
ServiceMgr +=SCT_ConfigurationConditionsSvc()


from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
ServiceMgr +=SCT_ConditionsSummarySvc()

SCT_ConditionsSummary=ServiceMgr.SCT_ConditionsSummarySvc
SCT_ConditionsSummary.ConditionsServices=["SCT_ModuleVetoSvc", "SCT_ConfigurationConditionsSvc","SCT_TdaqEnabledSvc"]

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummaryTestAlg
job+= SCT_ConditionsSummaryTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 40341 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  = 1204216576 #found valid in db browser?
theApp.EvtMax                   = 1

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
