#
# Job Options for reading LAr Alignments from COOL using local SQLite database
#

## basic job configuration
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

from LArAlignmentAlgs.LArAlignmentAlgsConf import LArAlignDbAlg
LArAlignDbAlg = LArAlignDbAlg()
topSequence += LArAlignDbAlg

# Here mytest.db is the name of SQLite file created by this job
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=laralign-test.db;dbname=OFLP200"
svcMgr.IOVDbSvc.Folders += [ "/LAR/Align  <tag>LARAlign_TEST</tag>" ]
    
svcMgr.MessageSvc.OutputLevel = 4
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.MessageSvc.infoLimit   = 100000

LArAlignDbAlg.OutputLevel     = 2
svcMgr.IOVDbSvc.OutputLevel   = 2

#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool



