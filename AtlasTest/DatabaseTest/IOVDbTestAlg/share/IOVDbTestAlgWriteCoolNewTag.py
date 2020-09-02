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

# Set time to register - used for IOVDbTestAmdbCorrection
IOVDbTestAlg.RegTime       = 0
# IOVDbTestAlg.CreateExtraChanns=True

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
IOVDbTestAlg.WriteCondObjs = True
# Choose the following to register cond objects in the IOVDB. 
IOVDbTestAlg.RegisterIOV   = True
# Write out attribute list with cool version
IOVDbTestAlg.ReadWriteCool = True
# Set the tag value
IOVDbTestAlg.TagID   = "COOL-TEST-NEW"
# Print lumiblock as well
IOVDbTestAlg.PrintLB = True
# Write out folders with new tag
IOVDbTestAlg.WriteNewTag = True

#--------------------------------------------------------------
# For IOVDb: specify dbConnection with COOL and oracle specified
#
#   Default is to use oracle schema named TESTCOOL, to override this use
#     athena  'TESTCOOL="<your test db name>"' IOVDbTestAlgWriteCool.py
#
# uncomment this to use a local SQLite file instead
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

#--------------------------------------------------------------
# Options for IOVRegistrationSvc
import RegistrationServices.IOVRegistrationSvc


# Dont set the tag for ALL folders
# in COOL folder tags must be globally unique
regSvc = svcMgr.IOVRegistrationSvc
# regSvc.IOVDbTag   = "DC1"

# Select the following to delete and recreate the folders. Default is
# NOT to recreate
regSvc.RecreateFolders = False
# The following set the interval for each of the IOVDbTest folders
# regSvc.BeginRun   = 4
# Max EndRun: 0x7FFFFFFF
# regSvc.EndRun     = 2147483647 
# regSvc.EndRun     = 3 
# regSvc.BeginLB = 0
# Max EndLB: 0xFFFFFFFF
# regSvc.EndLB   = 4294967295
# regSvc.IOVDbTag   = "DC1"

# extra write list to test all AttributeList types
# IOVDbTestAlg.FancyList=True
# regSvc.OverrideNames+=['FanSmallString','FanBigString']
# regSvc.OverrideTypes+=['String255','String64k']



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
theApp.EvtMax                   = 25

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------

#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------

# Append data to same file
svcMgr.PoolSvc.FileOpen = "update"

theApp.OutStream = []

from AthenaCommon.AppMgr import ToolSvc
IOVDbTestAlg.StreamName = "CondStream2"
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
ToolSvc += AthenaOutputStreamTool("CondStream2",OutputFile = "SimplePoolFile.root")
