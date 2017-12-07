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
globalflags.DetDescrVersion="ATLAS-R1-2012-03-00-00"
globalflags.ConditionsTag="COMCOND-BLKPA-RUN1-09"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
print globalflags

#--------------------------------------------------------------
# Set up conditions
#--------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.projectName.set_Value_and_Lock("data12_8TeV")

from IOVSvc.IOVSvcConf import CondSvc
ServiceMgr += CondSvc()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

from IOVDbSvc.CondDB import conddb

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SensorsCondAlg
condSeq += SCT_SensorsCondAlg( "SCT_SensorsCondAlg" )

# Load IOVDbSvc
IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.GlobalTag=globalflags.ConditionsTag()
IOVDbSvc.OutputLevel = 3
conddb.dbdata="COMP200"
conddb.addFolderWithTag("SCT_OFL","/SCT/Sensors","SctSensors-Sep03-14", className="CondAttrListCollection")

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

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SensorsSvc
ServiceMgr +=SCT_SensorsSvc()

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SensorsTestAlg
job+= SCT_SensorsTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob


ServiceMgr.EventSelector.RunNumber  = 140975
theApp.EvtMax                   = 1
