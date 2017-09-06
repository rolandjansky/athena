import AthenaCommon.AtlasUnixStandardJob
import os

#get db password
authPath=os.environ["CORAL_AUTH_PATH"] +"/authentication.xml"
f=open(authPath)
signatureFound=False
pw=""
for line in f:
  if "ATLAS_COOL_READER_U" in line:
   signatureFound=True
  if (signatureFound and "password" in line):
   tokens=line.split('"')
   pw=tokens[3]
   break
#print pw

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

from AthenaCommon.GlobalFlags import globalflags 
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas" 
globalflags.InputFormat="pool" 
globalflags.DataSource="data" 

eventInfoKey = "ByteStreamEventInfo"
if globalflags.DataSource()=="geant4":
  eventInfoKey = "McEventInfo"

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

#------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#------------------------------------------------------------
# Local stuff
#------------------------------------------------------------
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
ServiceMgr +=SCT_MonitorConditionsSvc(EventInfoKey=eventInfoKey)

SCT_MonitorConditions=ServiceMgr.SCT_MonitorConditionsSvc
#SCT_MonitorConditions.BadModuleIdentifiers=["1","2"]
SCT_MonitorConditions.OutputLevel = DEBUG
SCT_MonitorConditions.WriteCondObjs = False
SCT_MonitorConditions.RegisterIOV   = False
SCT_MonitorConditions.ReadWriteCool = True
#SCT_MonitorConditions.TagID4List = "list_v1"
#SCT_MonitorConditions.TagID4Eff = "eff_v1"
##
## if ManualIOV = false, then the IOV to store into DB is
##    the one corresponding to the current run
## if ManualIOV = true, the user has to provide the
##    run range for the proper IOV
SCT_MonitorConditions.ManualIOV = True
SCT_MonitorConditions.BeginRun = 310809
SCT_MonitorConditions.EndRun   = 310809
SCT_MonitorConditions.VersionN   = 0

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsTestAlg
job+= SCT_MonitorConditionsTestAlg()

SCT_MonitorTest=job.SCT_MonitorConditionsTestAlg
SCT_MonitorTest.OutputLevel      = 2
SCT_MonitorTest.RunNumber    = 310809
SCT_MonitorTest.EventNumber  = 5

#--------------------------------------------------------------
# Load IOVDbSvc
#     - access to COOL -
#--------------------------------------------------------------
#from IOVDbSvc.CondDB import conddb
IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.GlobalTag='CONDBR2-BLKPA-2017-10'
#IOVDbSvc.Folders+=["<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_SCT;dbname=OFLP200;user=ATLAS_COOL_READER_U;password="+pw+"</dbConnection>/SCT/Derived/Monitoring <tag>SctDerivedMonitoring-NOMINAL</tag>" ]
#IOVDbSvc.Folders += ["<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_SCT;dbname=OFLP200;user=ATLAS_COOL_READER_U</dbConnection>/SCT/Derived/NoiseOccupancy <tag>SctDerivedMonitoring-NOMINAL</tag>" ]
#IOVDbSvc.Folders += ["<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_SCT;dbname=OFLP200;user=ATLAS_COOL_READER_</dbConnection>/SCT/Derived/Monitoring <tag>SctDerivedMonitoring-NOMINAL</tag>" ]

from IOVDbSvc.CondDB import conddb
conddb.addFolderWithTag("SCT_OFL","/SCT/Derived/Monitoring","SctDerivedMonitoring-RUN2-UPD4-005")

import RegistrationServices.IOVRegistrationSvc
regSvc = ServiceMgr.IOVRegistrationSvc
regSvc.RecreateFolders = False

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------
# Check the dictionary in memory for completeness
ServiceMgr.AthenaSealSvc.CheckDictionary = TRUE

#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 310809
theApp.EvtMax                       = 10

#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------
#theApp.OutStream = []

from AthenaCommon.AppMgr import ToolSvc
SCT_MonitorConditions.StreamName = "CondStream2"
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
ToolSvc += AthenaPoolOutputStreamTool("CondStream2",OutputFile = "SimplePoolFile.root")

MessageSvc.OutputLevel = INFO
