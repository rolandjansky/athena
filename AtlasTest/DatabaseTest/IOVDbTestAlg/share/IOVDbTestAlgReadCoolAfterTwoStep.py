###############################################################
#
# Job options file for IOVDbTestAlg:  see doc in README
#
#==============================================================
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support - to read conditions in POOL, not events
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from IOVDbTestAlg.IOVDbTestAlgConf import IOVDbTestAlg
topSequence += IOVDbTestAlg( "IOVDbTestAlg" )

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
IOVDbTestAlg.OutputLevel           = DEBUG
svcMgr.IOVDbSvc.OutputLevel        = INFO

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
IOVDbTestAlg.WriteCondObjs   = False
# Choose the following to register cond objects in the IOVDB. 
IOVDbTestAlg.RegisterIOV     = False
# Do not register AttributeList - only for cool version
IOVDbTestAlg.ReadWriteCool   = True
# Do not check AttributeList for two-step write/reg
IOVDbTestAlg.TwoStepWriteReg = True



#--------------------------------------------------------------
#   COOL ConditionsDB tests
# For IOVDb: specify dbConnection with COOL and oracle specified
#
# uncomment this to use a local SQLite file instead
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

tag    = "_COOL-TEST-001"
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestMDTEleMap <tag>MDTEleMap" + tag + "</tag>" ]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAMDBCorrection <tag>AmdbCorrection"  + tag + "</tag>"]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAttrList  <tag>AttrList"  + tag + "</tag>"]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAttrListColl  <tag>AttrListColl"  + tag + "</tag>"]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestMDTEleMapColl <tag>MDTEleMapColl"  + tag + "</tag>"]

#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
# Number of events to be processed (default is 10)
#ApplicationMgr.EvtMax = 1
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 5
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 30
#
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = False
#

