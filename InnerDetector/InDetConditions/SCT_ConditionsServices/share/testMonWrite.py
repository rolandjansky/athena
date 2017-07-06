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

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo="atlas"
# --- set defaults
globalflags.DataSource="geant4"
globalflags.InputFormat="pool"
# --- default is zero luminosity
globalflags.Luminosity="zero"
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

#------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#------------------------------------------------------------
# Local stuff
#------------------------------------------------------------

eventInfoKey = "ByteStreamEventInfo"
if globalflags.DataSource()=="geant4":
    eventInfoKey = "McEventInfo"

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
ServiceMgr +=SCT_MonitorConditionsSvc(EventInfoKey=eventInfoKey)

SCT_MonitorConditions=ServiceMgr.SCT_MonitorConditionsSvc
#SCT_MonitorConditions.BadModuleIdentifiers=["1","2"]
SCT_MonitorConditions.OutputLevel = DEBUG
SCT_MonitorConditions.WriteCondObjs = True
SCT_MonitorConditions.RegisterIOV   = True
SCT_MonitorConditions.ReadWriteCool = True
#SCT_MonitorConditions.TagID4List = "list_v1"
#SCT_MonitorConditions.TagID4Eff = "eff_v1"
##
## if ManualIOV = false, then the IOV to store into DB is
##    the one corresponding to the current run
## if ManualIOV = true, the user has to provide the
##    run range for the proper IOV
SCT_MonitorConditions.ManualIOV = True
SCT_MonitorConditions.BeginRun = 3070
SCT_MonitorConditions.EndRun   = 3079
SCT_MonitorConditions.VersionN   = 0

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsTestAlg
job+= SCT_MonitorConditionsTestAlg()

SCT_MonitorTest=job.SCT_MonitorConditionsTestAlg
SCT_MonitorTest.OutputLevel      = 2
# SCT_MonitorTest.TestWrite = True
SCT_MonitorTest.RunNumber    = 1
SCT_MonitorTest.EventNumber  = 5

#--------------------------------------------------------------
# Load IOVDbSvc
#     - access to COOL -
#--------------------------------------------------------------
#from IOVDbSvc.CondDB import conddb
import IOVDbSvc.IOVDb

ServiceMgr.IOVDbSvc.GlobalTag='OFLCOND-CSC-00-00-00'

#try:
#    ServiceMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
#    ServiceMgr.IOVDbSvc.dbConnection  += "TESTCOOL"
#    ServiceMgr.IOVDbSvc.dbConnection  += ":ATLAS_COOL_MONTEST:pwd4sctmon"
#except:
#    ServiceMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOL_MONTEST"

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

ServiceMgr.EventSelector.RunNumber  = 1
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

MessageSvc.OutputLevel = DEBUG
