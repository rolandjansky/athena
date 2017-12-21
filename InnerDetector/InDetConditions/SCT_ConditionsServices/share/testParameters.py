###############################################################
#
# Job options file to test DCS conditions tool
#
#==============================================================

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc
#from AthenaCommon.AppMgr import theApp

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

from RecExConfig.RecFlags import rec
rec.projectName.set_Value_and_Lock("data12_8TeV")

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

#--------------------------------------------------------------
# Load DCSConditions Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg(OutputLevel=2)

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsParameterTestAlg
topSequence+= SCT_ConditionsParameterTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsParameterSvc
ServiceMgr += SCT_ConditionsParameterSvc()

#SCT_DCSConditionsSvc=ServiceMgr.SCT_DCSConditionsSvc
SCT_ConditionsParameterSvc.AttrListCollFolders=["/SCT/DAQ/Configuration/Chip"]


#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
#ServiceMgr+= EventSelector()
#ServiceMgr.EventSelector.FirstEvent = 1
#ServiceMgr.EventSelector.EventsPerRun = 5
ServiceMgr.EventSelector.RunNumber = 215643
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
#ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
# increment of 3 minutes
ServiceMgr.EventSelector.TimeStampInterval = 180

theApp.EvtMax                   = 10

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = 3

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
conddb.dbdata="COMP200"
IOVDbSvc.GlobalTag=globalflags.ConditionsTag()
IOVDbSvc.OutputLevel = 3
conddb.addFolder('',"<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Chip")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/ROD")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Geog")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/RODMUR")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/MUR")
