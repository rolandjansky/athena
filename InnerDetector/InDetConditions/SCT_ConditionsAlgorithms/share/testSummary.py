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
globalflags.DetDescrVersion="ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
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

# Disable SiLorentzAngleSvc to remove
# ERROR ServiceLocatorHelper::createService: wrong interface id IID_665279653 for service
ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool'].LorentzAngleSvc=""
ServiceMgr.GeoModelSvc.DetectorTools['SCT_DetectorTool'].LorentzAngleSvc=""

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2017-06"
print "conddb.dbdata", conddb.dbdata
IOVDbSvc.OutputLevel = 3

from SCT_ConditionsServices.SCT_TdaqEnabledSvcSetup import SCT_TdaqEnabledSvcSetup
sct_TdaqEnabledSvcSetup = SCT_TdaqEnabledSvcSetup()
sct_TdaqEnabledSvcSetup.setup()

from SCT_ConditionsServices.SCT_ConfigurationConditionsSvcSetup import SCT_ConfigurationConditionsSvcSetup
sct_ConfigurationConditionsSvcSetup = SCT_ConfigurationConditionsSvcSetup()
sct_ConfigurationConditionsSvcSetup.setup()

conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Geog", "/SCT/DAQ/Config/Geog") # Needed for cabling
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/RODMUR", "/SCT/DAQ/Config/RODMUR") # Needed for cabling
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/ROD", "/SCT/DAQ/Config/ROD") # Needed for cabling

from SCT_ConditionsServices.SCT_ModuleVetoSvcSetup import SCT_ModuleVetoSvcSetup
sct_ModuleVetoSvcSetup = SCT_ModuleVetoSvcSetup()
sct_ModuleVetoSvcSetup.setUseDB(False)
sct_ModuleVetoSvcSetup.setup()
SCT_ModuleVetoSvc=sct_ModuleVetoSvcSetup.getSvc()
SCT_ModuleVetoSvc.BadModuleIdentifiers=["1", "2"]

from SCT_ConditionsServices.SCT_ConditionsSummarySvcSetup import SCT_ConditionsSummarySvcSetup
sct_ConditionsSummarySvcSetup = SCT_ConditionsSummarySvcSetup()
sct_ConditionsSummarySvcSetup.setup()
SCT_ConditionsSummarySvc = sct_ConditionsSummarySvcSetup.getSvc()
SCT_ConditionsSummarySvc.ConditionsServices=[sct_ModuleVetoSvcSetup.getSvcName(),
                                             sct_ConfigurationConditionsSvcSetup.getSvcName(),
                                             sct_TdaqEnabledSvcSetup.getSvcName()]

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ConditionsSummaryTestAlg
job+= SCT_ConditionsSummaryTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 310809
import time, calendar
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
theApp.EvtMax                   = 1

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
