###############################################################
#
# Job options file for TestCoolRecExample.py
# Reads a user-defined set of data written by TestCoolRecWriter
#
#==============================================================
import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb
svcMgr.IOVDbSvc.OutputLevel=3
svcMgr.IOVDbSvc.ManageConnections=False

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaDBTestRec.AthenaDBTestRecConf import TestCoolRecRead
theTestCoolRecRead=TestCoolRecRead(OutputLevel=INFO)
topSequence+=theTestCoolRecRead

#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
# and timestampsjob
svcMgr.EventSelector.RunNumber         = 10000
svcMgr.EventSelector.EventsPerRun      = 20
svcMgr.EventSelector.FirstEvent        = 1
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
svcMgr.EventSelector.InitialTimeStamp  = 0
# increment of 3 minutes
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 400

# folder configuration to be used
include("CoolReadConfig.py")
