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

from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
ToolSvc += IOVDbMetaDataTool( "IOVDbMetaDataTool" )
IOVDbMetaDataTool.OutputLevel      = INFO

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
#svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=mytest.db;X:TESTCOOL"
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

#--------------------------------------------------------------
#   MySQL ConditionsDB
# For IOVDb: set database name, and tag for data to be read
#IOVDbSvc.DBname      = "ConditionsDB_Pool_test"
#IOVDbSvc.GlobalTag   = "DC1"
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
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
# svcMgr.EventSelector.FirstLB=0
# svcMgr.EventSelector.EventsPerLB=3
theApp.EvtMax                   = 30
#
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = False
#
# Must list the folders to be used for reading
#
# Implicit list of folders without local tag
#IOVDbSvc.Folders += [ "/IOVDbTest/*" ]

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
#folder = "/IOVDbTest/IOVDbTestFancyList"
#tag = "FancyList" + tagSuffix
#IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
#IOVDbTestAlg.FancyList=True


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------

theApp.OutStream = []

from AthenaCommon.AppMgr import ToolSvc
IOVDbTestAlg.StreamName = "CondStream2"
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
ToolSvc += AthenaOutputStreamTool("CondStream2",OutputFile = "SimplePoolFile.root")

include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )

print (fullItemList)

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", "SimpleEventPoolFile.root", noTag=True )
# List of DO's to write out
Stream1.ItemList   += fullItemList

# List of folders to  write to file meta data
svcMgr.IOVDbSvc.FoldersToMetaData = []
svcMgr.IOVDbSvc.FoldersToMetaData += [ "/IOVDbTest/*" ]
#svcMgr.IOVDbSvc.FoldersToMetaData += [ "/IOVDbTest/IOVDbTestAMDBCorrection" ]
#svcMgr.IOVDbSvc.FoldersToMetaData += ["/IOVDbTest/IOVDbTestAttrList" ]
#svcMgr.IOVDbSvc.FoldersToMetaData += [ "/IOVDbTest/IOVDbTestAttrListColl" ]
#svcMgr.IOVDbSvc.FoldersToMetaData += [ "/IOVDbTest/IOVDbTestMDTEleMapColl" ]

#--------------------------------------------------------------
# Create simulation and digitization parameters and add them to the
# file meta data
#--------------------------------------------------------------

from IOVDbMetaDataTools import ParameterDbFiller
# Set run numbers and parameters
beginRun    = 0
endRun      = 99999
simParams   = []
digitParams = []
simParams  += ['k_a']
simParams  += ['v_a']
simParams  += ['k_b']
simParams  += ['v_b']
simParams  += ['k_c']
simParams  += ['v_c1 v_c1 v_c1']
digitParams  += ['k_d_a']
digitParams  += ['v_d_a']
digitParams  += ['k_d_b']
digitParams  += ['v_d_b']
digitParams  += ['k_d_c']
digitParams  += ['v_d_c1 v_d_c1 v_d_c1']

# Create simulation parameters db
dbFiller = ParameterDbFiller.ParameterDbFiller()
# set iov
dbFiller.setBeginRun(beginRun)
dbFiller.setEndRun(endRun)
# set parameters
for i in range(len(simParams)//2):
    dbFiller.addSimParam(simParams[2*i], simParams[2*i+1])
# generate db
dbFiller.genSimDb()

# create digit parameters db
# set parameters
for i in range(len(digitParams)//2):
    dbFiller.addDigitParam(digitParams[2*i], digitParams[2*i+1])
# generate db
dbFiller.genDigitDb()

# Set properties for meta data
folder = "/Simulation/Parameters"
dbConnection = "sqlite://;schema=SimParams.db;dbname=SIMPARAM"
svcMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
svcMgr.IOVDbSvc.FoldersToMetaData += [ folder ]

folder = "/Digitization/Parameters"
dbConnection = "sqlite://;schema=DigitParams.db;dbname=DIGPARAM"
svcMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
svcMgr.IOVDbSvc.FoldersToMetaData += [ folder ]

# To force the reading in of the folders to be written as file meta
# data, need following:
svcMgr.IOVSvc.partialPreLoadData = True



