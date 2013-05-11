###############################################################
#
# Job options file for IOVDbTestAlg:  see doc in README
#
#==============================================================
#use McEventSelector
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

# Set handler for traceback
#gbl.AthenaServices.SetFatalHandler(438)

#import password for devdb10 from devdb10pwd.py
from devdb10_pwd import devdb10pwd

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
theApp.isOnline=True
isOnline=True
#--------------------------------------------------------------
# Load POOL support - to read conditions in POOL, not events
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

#--------------------------------------------------------------
# Make sure AthenaSealSvc is loaded for dict check
#--------------------------------------------------------------
# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = TRUE

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from IOVDbTestAlg.IOVDbTestAlgConf import IOVDbTestAlg
topSequence += IOVDbTestAlg( "IOVDbTestAlg" )
#--------------------------------------------------------------
# from AthenaDBTestRec.AthenaDBTestRecConf import TestCoolRecRead
# theTestCoolRecRead=TestCoolRecRead(OutputLevel=INFO)
# topSequence+=theTestCoolRecRead
#--------------------------------------------------------------s

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.MessageSvc.infoLimit   = 100000
svcMgr.MessageSvc.Format      = "% F%30W%S%7W%R%T %0W%M"
IOVDbTestAlg.OutputLevel      = DEBUG
#svcMgr.IOVDbSvc.OutputLevel   = DEBUG
#svcMgr.IOVSvc.OutputLevel     = DEBUG
#svcMgr.TagInfoMgr.OutputLevel = DEBUG
svcMgr.ProxyProviderSvc.OutputLevel = DEBUG
svcMgr.AthenaPoolCnvSvc.OutputLevel = VERBOSE

svcMgr.IOVSvc.partialPreLoadData     = False
svcMgr.IOVDbSvc.OnlineMode   		 = True
#--------------------------------------------------------------
 
IOVDbTestAlg.WriteCondObjs = False		# Choose the following to write out cond objects .

IOVDbTestAlg.RegisterIOV   = False		# Choose the following to register cond objects in the IOVDB. 

IOVDbTestAlg.ReadWriteCool = True		# Do not register AttributeList - only for cool version

IOVDbTestAlg.PrintLB = True				# Print lumiblock as well
IOVDbTestAlg.online = True				# Print lumiblock as well


#--------------------------------------------------------------
#   COOL ConditionsDB tests
# For IOVDb: specify dbConnection with COOL and oracle specified

try:
    svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
    svcMgr.IOVDbSvc.dbConnection  += TESTCOOL
    svcMgr.IOVDbSvc.dbConnection  += ":ATLAS_COOLTEST:%s" %devdb10pwd
except:
	svcMgr.IOVDbSvc.dbConnection  = "oracle://devdb10;dbname=TESTCOOL;schema=ATLAS_COOLTEST;user=ATLAS_COOLTEST;password=%s" %devdb10pwd

# uncomment this to use a local SQLite file instead
#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

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
svcMgr.EventSelector.EventsPerLB       = 5
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
# svcMgr.EventSelector.FirstLB         = 0
# svcMgr.EventSelector.EventsPerLB     = 3
theApp.EvtMax                          = 50 


# Explicit list of folders with tags

tagSuffix = "_COOL-TEST-001"
folder    = "/IOVDbTest/IOVDbTestMDTEleMap"
tag       = "MDTEleMap" + tagSuffix


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

# #CondDBCool_Schema0="<db>sqlite://X;schema=refc1301.db;dbname=REFC1301</db>"
# #CondDBCool_Schema1="<db>sqlite://X;schema=mytest.db;dbname=REFC1301</db>"
# CondDBCool_Schema1="<db>sqlite://X;schema=mytest.db;dbname=TESTCOOL</db>"
# #svcMgr.IOVDbSvc.Folders+=[CondDBCool_Schema1 + "/ID/CORACOOL <tag>IDCORACOOL-tag1</tag>"]
# 
# svcMgr.IOVDbSvc.Folders += ["/ID/CORACOOL <tag>IDCORACOOL-tag1</tag>"]
# theTestCoolRecRead.Folders+=["/ID/CORACOOL"]
# theTestCoolRecRead.FTypes+=[2]
