import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

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

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------

IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-18"

### Use COOL database for SCT_ModuleVetoSvc
useDB = True # False

from SCT_ConditionsTools.SCT_ModuleVetoToolSetup import SCT_ModuleVetoToolSetup
sct_ModuleVetoToolSetup = SCT_ModuleVetoToolSetup()
if useDB:
    sct_ModuleVetoToolSetup.setFolderTag("SCTManualBadModules-000-00")

sct_ModuleVetoToolSetup.setUseDB(useDB)
sct_ModuleVetoToolSetup.setup()
SCT_ModuleVetoTool = sct_ModuleVetoToolSetup.getTool()
if useDB:
    SCT_ModuleVetoTool.BadModuleIdentifiers=["database"]
else:
    SCT_ModuleVetoTool.BadModuleIdentifiers=["1", "2"]

SCT_ModuleVetoTool.OutputLevel=DEBUG

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ModuleVetoTestAlg
job+= SCT_ModuleVetoTestAlg(ModuleVetoTool=SCT_ModuleVetoTool)


import AthenaCommon.AtlasUnixGeneratorJob


ServiceMgr.EventSelector.RunNumber = 300000 # MC16c 2017 run number
ServiceMgr.EventSelector.InitialTimeStamp = 1500000000 # MC16c 2017 time stamp
theApp.EvtMax = 20
