###############################################################
#
# Job options file for IOVDbTestAlg:  see doc in README
#
#==============================================================
## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the ToolSvc
from AthenaCommon.AppMgr import ToolSvc

#svcMgr.EventSelector.InputCollections = [ "SimplePoolFile1.root" ]
svcMgr.EventSelector.InputCollections = [ "SimpleEventPoolFile.root" ]


from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
ToolSvc += IOVDbMetaDataTool( "IOVDbMetaDataTool" )
# IOVDbMetaDataTool.OutputLevel      = DEBUG

from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]

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
#svcMgr.IOVSvc.OutputLevel          = DEBUG

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
IOVDbTestAlg.WriteCondObjs = False
# Choose the following to register cond objects in the IOVDB. 
IOVDbTestAlg.RegisterIOV   = False
# Do not register AttributeList - only for cool version
IOVDbTestAlg.ReadWriteCool = True
# Print lumiblock as well
IOVDbTestAlg.PrintLB = True

# Read conditions from file meta data - ignore non-attribute lists
#IOVDbTestAlg.ReadFromFileMetaData = True


#--------------------------------------------------------------
#   COOL ConditionsDB tests
# For IOVDb: specify dbConnection with COOL and oracle specified
#
# uncomment this to use a local SQLite file instead
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

# Reading from events we do not use over-ride tags because they should
# come from the input Event.

svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestMDTEleMap"      ]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAMDBCorrection" ]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAttrList"       ]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAttrListColl"   ]
svcMgr.IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestMDTEleMapColl"  ]
svcMgr.IOVDbSvc.Folders += [ "/Simulation/Parameters"  ]
svcMgr.IOVDbSvc.Folders += [ "/Digitization/Parameters"  ]


# # Explicit list of folders with tags
# tagSuffix = "_COOL-TEST-001"
# folder = "/IOVDbTest/IOVDbTestMDTEleMap"
# tag = "MDTEleMap" + tagSuffix
# svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
# folder = "/IOVDbTest/IOVDbTestAMDBCorrection"
# tag = "AmdbCorrection" + tagSuffix
# svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
# folder = "/IOVDbTest/IOVDbTestAttrList"
# tag = "AttrList" + tagSuffix
# svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
# folder = "/IOVDbTest/IOVDbTestAttrListColl"
# tag = "AttrListColl" + tagSuffix
# svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
# folder = "/IOVDbTest/IOVDbTestMDTEleMapColl"
# tag = "MDTEleMapColl" + tagSuffix
# svcMgr.IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]

# Implicit list for the rest
#IOVDbSvc.Folders += [ "/IOVDbTest/IOVDbTestAttr*" ]

# optional extra folder for the Fancy AttributeList
#folder = "/IOVDbTest/IOVDbTestFancyList"
#tag = "FancyList" + tagSuffix
#IOVDbSvc.Folders += [ folder + "<tag>" + tag + "</tag>" ]
#IOVDbTestAlg.FancyList=True



