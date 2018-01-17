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
globalflags.DataSource="geant4"
print globalflags

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
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-18"

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoSvc
ServiceMgr +=SCT_ModuleVetoSvc()

SCT_ModuleVeto=ServiceMgr.SCT_ModuleVetoSvc

### Use COOL database for SCT_ModuleVetoSvc
useDB = True # False
if useDB:
    SCT_ModuleVeto.BadModuleIdentifiers=["database"]
    from IOVSvc.IOVSvcConf import CondSvc
    ServiceMgr += CondSvc()
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoCondAlg
    condSeq += SCT_ModuleVetoCondAlg( "SCT_ModuleVetoCondAlg" )
    conddb.addFolderWithTag("SCT_OFL", "/SCT/Manual/BadModules", "SCTManualBadModules-000-00", className="AthenaAttributeList")
else:
    SCT_ModuleVeto.BadModuleIdentifiers=["1", "2"]

SCT_ModuleVetoSvc.OutputLevel=DEBUG

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoTestAlg
job+= SCT_ModuleVetoTestAlg()



import AthenaCommon.AtlasUnixGeneratorJob


ServiceMgr.EventSelector.RunNumber  = 300000 # MC16c 2017 run number
ServiceMgr.EventSelector.InitialTimeStamp  = 1500000000 # MC16c 2017 time stamp
theApp.EvtMax                   = 1
