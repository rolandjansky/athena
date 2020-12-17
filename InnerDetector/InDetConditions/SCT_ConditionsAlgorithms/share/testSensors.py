import AthenaCommon.AtlasUnixStandardJob

# Setup logger
from AthenaCommon.Logging import logging
msg = logging.getLogger("testSensors")
msg.setLevel(logging.INFO)

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
numThreads = jobproperties.ConcurrencyFlags.NumThreads()
if numThreads > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R1-2012-03-00-00"
globalflags.ConditionsTag="COMCOND-BLKPA-RUN1-09"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
msg.info(globalflags)

#--------------------------------------------------------------
# Set up conditions
#--------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.projectName.set_Value_and_Lock("data12_8TeV")

# Load IOVDbSvc
IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.GlobalTag=globalflags.ConditionsTag()
IOVDbSvc.OutputLevel = 3
from IOVDbSvc.CondDB import conddb
conddb.dbdata="COMP200"

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

if conddb.dbdata=="COMP200":
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    InDetGeometryFlags.useDynamicAlignFolders = False

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from SCT_ConditionsTools.SCT_SensorsToolSetup import SCT_SensorsToolSetup
sct_SensorsToolSetup = SCT_SensorsToolSetup()
sct_SensorsToolSetup.setFolderTag("SctSensors-Sep03-14")
sct_SensorsToolSetup.setup()

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_SensorsTestAlg
job+= SCT_SensorsTestAlg(name="SCT_SensorsTestAlg",
                         SCT_SensorsTool=sct_SensorsToolSetup.getTool())

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
    job.SCT_SensorsTestAlg.Cardinality = numThreads

import AthenaCommon.AtlasUnixGeneratorJob


ServiceMgr.EventSelector.RunNumber  = 140975
theApp.EvtMax                   = 20
