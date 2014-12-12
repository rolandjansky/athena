include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")


from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber	= 10000000
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	= 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1
theApp.EvtMax = 1

include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
svcMgr.IOVDbSvc.GlobalTag="COMCOND-005-02"
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

conddb.addFolder("LAR","/LAR/ElecCalibOfl/OFCBin/Dummy")
conddb.addFolder("LAR","/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples3bins17phases<key>LArOFCIn</key>")

from LArCalibUtils.LArCalibUtilsConf import LArOFPhasePicker
LArOFPhasePicker = LArOFPhasePicker("LArOFPhasePicker")
LArOFPhasePicker.KeyPhase = "LArOFCPhase"
LArOFPhasePicker.KeyOFC_new = "LArOFC"
LArOFPhasePicker.KeyOFC = "LArOFCIn"
LArOFPhasePicker.GroupingType = "ExtendedSubDetector"
LArOFPhasePicker.OutputLevel = DEBUG
LArOFPhasePicker.DefaultPhase=1
topSequence += LArOFPhasePicker
#
# Make an output
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
Folder = "/LAR/ElecCalibOnl/OFC"
ObjectSpec = [ "LArOFCComplete#LArOFC#"+Folder ]
TagSpec = []
OutputDB = "sqlite://X;schema=OFCnew.db;dbname=CONDBR2"
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","./ofc_picked.pool.root", 
                                           ObjectSpec,TagSpec,True)
theOutputConditionsAlg.Run1 = 0
theOutputConditionsAlg.Run2 = 0

topSequence += theOutputConditionsAlg

svcMgr.IOVDbSvc.OutputLevel = VERBOSE

from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder=True

svcMgr.IOVDbSvc.dbConnection  = OutputDB

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"
   
