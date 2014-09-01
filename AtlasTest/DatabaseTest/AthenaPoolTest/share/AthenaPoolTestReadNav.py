###############################################################
#
# Job options file
#
## @file AthenaPoolTestReadNav.py
##
## @brief For Athena POOL test: read back AthenaPoolTest data objects
## from two files using navigation
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

# To read back from two files:
svcMgr.EventSelector.InputCollections       = [ "SimplePoolFile1.root" ]
svcMgr.AthenaPoolAddressProviderSvc.BackNavigation = True
#svcMgr.EventSelector.BackNavigation         = TRUE
#svcMgr.EventSelector.OutputLevel            = 2
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 5
#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolTest.AthenaPoolTestConf import AthenaPoolTestDataReader
topSequence += AthenaPoolTestDataReader( "AthenaPoolTestDataReader" )


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc = Service( "MessageSvc" )
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000
#rds ClassIDSvc = Service( "ClassIDSvc" )
#rds ClassIDSvc.OutputLevel = 3
AthenaPoolTestDataReader.OutputLevel = DEBUG

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = INFO

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
