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
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
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
IOVDbSvc.OutputLevel = 3

#ToolSvc = ServiceMgr.ToolSvc

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

conddb.addFolder("TDAQ", "/TDAQ/Resources/ATLAS/SCT/Robins", className="CondAttrListCollection")
from  SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledCondAlg
condSeq += SCT_TdaqEnabledCondAlg(name="SCT_TdaqEnabledCondAlg")

conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Chip", "/SCT/DAQ/Config/Chip", className="CondAttrListVec")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Module", "/SCT/DAQ/Config/Module", className="CondAttrListVec")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/MUR", "/SCT/DAQ/Config/MUR", className="CondAttrListVec") # Also for cabling
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationCondAlg
condSeq += SCT_ConfigurationCondAlg(name = "SCT_ConfigurationCondAlg",
                                    ReadKeyChannel = "/SCT/DAQ/Config/Chip",
                                    ReadKeyModule = "/SCT/DAQ/Config/Module",
                                    ReadKeyMur = "/SCT/DAQ/Config/MUR")

conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Geog", "/SCT/DAQ/Config/Geog") # Needed for cabling
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/RODMUR", "/SCT/DAQ/Config/RODMUR") # Needed for cabling
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/ROD", "/SCT/DAQ/Config/ROD") # Needed for cabling

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoSvc
ServiceMgr +=SCT_ModuleVetoSvc()
SCT_ModuleVeto=ServiceMgr.SCT_ModuleVetoSvc
SCT_ModuleVeto.BadModuleIdentifiers=["1","2"]

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
ServiceMgr +=SCT_ConfigurationConditionsSvc()


from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_CachedSummarySvc
ServiceMgr +=SCT_CachedSummarySvc()

SCT_CachedSummary=ServiceMgr.SCT_CachedSummarySvc
SCT_CachedSummary.ConditionsServices=["SCT_ModuleVetoSvc", "SCT_ConfigurationConditionsSvc","SCT_TdaqEnabledSvc"]

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_CachedSummaryTestAlg
job+= SCT_CachedSummaryTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber = 310809

# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
# increment of 3 minutes
ServiceMgr.EventSelector.TimeStampInterval = 180

theApp.EvtMax                   = 5

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
