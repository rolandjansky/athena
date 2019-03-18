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
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()

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

# Set up SCT cabling
runNumber=310809
rec.RunNumber.set_Value_and_Lock(runNumber)
from AthenaCommon.Include import include
include('InDetRecExample/InDetRecCabling.py')

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

from SCT_ConditionsTools.SCT_TdaqEnabledToolSetup import SCT_TdaqEnabledToolSetup
sct_TdaqEnabledToolSetup = SCT_TdaqEnabledToolSetup()
sct_TdaqEnabledToolSetup.setup()

from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
sct_ConfigurationConditionsToolSetup.setup()

conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Geog", "/SCT/DAQ/Config/Geog") # Needed for cabling
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/RODMUR", "/SCT/DAQ/Config/RODMUR") # Needed for cabling
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/ROD", "/SCT/DAQ/Config/ROD") # Needed for cabling

from SCT_ConditionsTools.SCT_ModuleVetoToolSetup import SCT_ModuleVetoToolSetup
sct_ModuleVetoToolSetup = SCT_ModuleVetoToolSetup()
sct_ModuleVetoToolSetup.setUseDB(False)
sct_ModuleVetoToolSetup.setup()
SCT_ModuleVetoTool=sct_ModuleVetoToolSetup.getTool()
SCT_ModuleVetoTool.BadModuleIdentifiers=["1", "2"]

from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
sct_ConditionsSummaryToolSetup.setup()
SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
SCT_ConditionsSummaryTool.ConditionsTools=[sct_ModuleVetoToolSetup.getTool().getFullName(),
                                           sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                           sct_TdaqEnabledToolSetup.getTool().getFullName()]

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ConditionsSummaryTestAlg
job+= SCT_ConditionsSummaryTestAlg(SCT_ConditionsSummaryTool=SCT_ConditionsSummaryTool)

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber = runNumber
import time, calendar
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
theApp.EvtMax                   = 20

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
