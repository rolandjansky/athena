include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
include("LArCalibProcessing/LArCalib_MinimalSetup.py")

if 'runNumber' not in dir():
    runNumber=2147483647

if not 'beginRun' in dir():
    beginRun=0


from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber = runNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


if not "doPrint" in dir():
    doPrint=True
if doPrint:
    theApp.EvtMax = 1
else:
    theApp.EvtMax = 5000

from LArConditionsTest.LArConditionsTestConf import LArCablingTest
topSequence+=LArCablingTest(Print=doPrint)



from AthenaCommon.AppMgr import ServiceMgr as svcMgr
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
 
#svcMgr.ChronoStatSvc.ChronoDestinationCout = True

svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True
#svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1

svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chronoStats.ascii"

svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

theApp.AuditAlgorithms = True

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

#MessageSvc = theApp.service( "MessageSvc" )
#MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"

svcMgr.IOVDbSvc.GlobalTag   = "COMCOND-BLKPA-006-05"
svcMgr.MessageSvc.OutputLevel=INFO
