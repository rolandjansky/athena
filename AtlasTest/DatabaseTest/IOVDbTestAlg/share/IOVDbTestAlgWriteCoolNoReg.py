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
# Private Application Configuration options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from IOVDbTestAlg.IOVDbTestAlgConf import IOVDbTestAlg
topSequence += IOVDbTestAlg( "IOVDbTestAlg" )
#/--------------------------------------------------------------

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
IOVDbTestAlg.OutputLevel           = DEBUG


# Set time to register - used for IOVDbTestAmdbCorrection
IOVDbTestAlg.RegTime       = 0

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
IOVDbTestAlg.WriteCondObjs = True
# Choose the following to register cond objects in the IOVDB. 
IOVDbTestAlg.RegisterIOV   = False
# Do not register AttributeList - only for cool version
IOVDbTestAlg.ReadWriteCool = True

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

theApp.OutStream = []


from AthenaCommon.AppMgr import ToolSvc
IOVDbTestAlg.StreamName = "CondStream1"
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
ToolSvc += AthenaOutputStreamTool("CondStream1",OutputFile = "SimplePoolFile.root")


