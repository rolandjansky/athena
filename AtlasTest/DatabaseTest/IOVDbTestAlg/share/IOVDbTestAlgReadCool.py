###############################################################
#
# Job options file for IOVDbTestAlg:  see doc in README
#
#==============================================================
#use McEventSelector
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
IOVDbTestAlg.WriteCondObjs = False
# Choose the following to register cond objects in the IOVDB. 
IOVDbTestAlg.RegisterIOV   = False
# Do not register AttributeList - only for cool version
IOVDbTestAlg.ReadWriteCool = True
# Print lumiblock as well
IOVDbTestAlg.PrintLB = True


#--------------------------------------------------------------
#   COOL ConditionsDB tests
# For IOVDb: specify dbConnection with COOL and oracle specified
#
# uncomment this to use a local SQLite file instead
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

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
svcMgr.EventSelector.EventsPerRun      = 10
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
# svcMgr.EventSelector.FirstLB=0
# svcMgr.EventSelector.EventsPerLB=3
theApp.EvtMax                   = 30

# Explicit list of folders with tags
tagSuffix = "_COOL-TEST-001"
folder = "/IOVDbTest/IOVDbTestMDTEleMap"
tag = "MDTEleMap" + tagSuffix
svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
folder = "/IOVDbTest/IOVDbTestAMDBCorrection"
tag = "AmdbCorrection" + tagSuffix
svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
folder = "/IOVDbTest/IOVDbTestAttrList"
tag = "AttrList" + tagSuffix
svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
folder = "/IOVDbTest/IOVDbTestAttrListColl"
tag = "AttrListColl" + tagSuffix
svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
folder = "/IOVDbTest/IOVDbTestMDTEleMapColl"
tag = "MDTEleMapColl" + tagSuffix
svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
# Implicit list for the rest
#IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAttr*" ]

# optional extra folder for the Fancy AttributeList
# folder = "/IOVDbTest/IOVDbTestFancyList"
# tag = "FancyList" + tagSuffix
# svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
# IOVDbTestAlg.FancyList=True


