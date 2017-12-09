
if not 'OutputSqliteFile' in dir():
   OutputSqliteFile="dspconfig.db"

DSPConfigFolder="/LAR/Configuration/DSPConfiguration"

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

from AthenaCommon.GlobalFlags import globalflags
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

from AthenaCommon.AppMgr import theApp, ServiceMgr 
include("LArCalibProcessing/LArCalib_MinimalSetup.py")


from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from IOVDbSvc.CondDB import conddb

svcMgr.IOVDbSvc.GlobalTag   = "LARCALIB-RUN2-00"
svcMgr.IOVDbSvc.DBInstance=""


from LArCalibTools.LArCalibToolsConf import LArFillDSPConfig

topSequence+=LArFillDSPConfig(Foldername=DSPConfigFolder)

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",["AthenaAttributeList#"+DSPConfigFolder,],["",],True)

svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+OutputSqliteFile+";dbname=CONDBR2"

    
from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = True
         

theApp.EvtMax = 1

from McEventSelector.McEventSelectorConf import McEventSelector
ServiceMgr += McEventSelector("EventSelector",RunNumber  = 2147483647)
#svcMgr.EventSelector.
#svcMgr.EventSelector.EventsPerRun      = 1
#svcMgr.EventSelector.FirstEvent = 1
#svcMgr.EventSelector.InitialTimeStamp  = 0
#svcMgr.EventSelector.TimeStampInterval = 1
