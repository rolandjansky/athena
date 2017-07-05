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
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
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

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsTestAlg
topSequence+= SCT_DCSConditionsTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
ServiceMgr += SCT_DCSConditionsSvc()

#SCT_DCSConditionsSvc=ServiceMgr.SCT_DCSConditionsSvc
SCT_DCSConditionsSvc.AttrListCollFolders=["/SCT/DCS/HV","/SCT/DCS/MODTEMP","/SCT/DCS/CHANSTAT","/SCT/DCS/MPS/LV"]

#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
#ServiceMgr+= EventSelector()
#ServiceMgr.EventSelector.FirstEvent = 1
#ServiceMgr.EventSelector.EventsPerRun = 5
ServiceMgr.EventSelector.RunNumber = 0
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# the value given here corresponds to Sat Oct 18 2008 16:04:41 UTC
ServiceMgr.EventSelector.InitialTimeStamp  = 1224345881
# increment of 3 minutes
ServiceMgr.EventSelector.TimeStampInterval = 180

theApp.EvtMax                   = 10

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = 3
#ServiceMgr.SCT_DCSConditionsSvc.OutputLevel = 3
#topSequence.SCT_DCSConditionsTestAlg.OutputLevel = 3

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
#IOVDbSvc.GlobalTag="HEAD"
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3
conddb.addFolder('',"<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/HV")
conddb.addFolder('',"<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/MODTEMP")
conddb.addFolder('',"<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/CHANSTAT")
conddb.addFolder('',"<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/MPS/LV")


#InDetSCT_ConditionsSummarySvc.ConditionsServices+=["InDetSCT_DCSConditionsSvc"]
#Temporary access to May Barrel COOL 2.0 data
#CoolSCTDCS='<dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOL_SCT;ATLAS_COOLPROD:DCSP200:ATLAS_COOL_READER</dbConnection>'
#CoolSCTDCS='<dbConnection>impl=cool;techno=sqlite;schema=ATLAS_COOL_SCT;ATLAS_COOLPROD:DCSP200:ATLAS_COOL_READER</dbConnection>'
#IOVDbSvc.Folders+= [ CoolSCTDCS + ' /SCT/DCS/HV' ]
#IOVDbSvc.Folders+= [ CoolSCTDCS + ' /SCT/DCS/MODTEMP' ]
#IOVDbSvc.OutputLevel =INFO
