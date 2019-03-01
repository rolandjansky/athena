useDB=True

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

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-18"
IOVDbSvc.OutputLevel = 3

if useDB:
    # Set up SCT_DCSConditionsTool and required conditions folders and conditions algorithms
    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setup()

# For SCT_SiliconConditionsSvc
from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
sct_SiliconConditionsToolSetup.setUseDB(useDB)
sct_SiliconConditionsToolSetup.setDcsTool(sct_DCSConditionsToolSetup.getTool())
sct_SiliconConditionsToolSetup.setup()

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_SiliconConditionsTestAlg
job+= SCT_SiliconConditionsTestAlg(SCT_SiliconConditionsTool=sct_SiliconConditionsToolSetup.getTool())

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 300000 # MC16c 2017 run number
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  = 1500000000
ServiceMgr.EventSelector.TimeStampInterval = 0
theApp.EvtMax                   = 20

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
