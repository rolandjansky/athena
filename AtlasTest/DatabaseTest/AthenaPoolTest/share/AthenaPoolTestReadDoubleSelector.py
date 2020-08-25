###############################################################
#
# Job options file
#
## @file AthenaPoolTestReadDoubleSelector.py
##
## @brief For Athena POOL test: read back AthenaPoolTest data objects 
##
## @author Miha Muskinja <miha.muskinja@cern.ch>
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

# Set that we are running MC+MC overlay in MT mode
from OverlayCommonAlgs.OverlayFlags import overlayFlags
overlayFlags.isDataOverlay.set_Value_and_Lock(False)
overlayFlags.isOverlayMT.set_Value_and_Lock(True)

#--------------------------------------------------------------
# Load POOL support for DoubleEventSelector
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble

#--------------------------------------------------------------
# Input options for DoubleEventSelector
#--------------------------------------------------------------

# Add in DoubleEventSelector
svcMgr.DoubleEventSelector.InputCollections = [ "SimplePoolFile1.root" ]
svcMgr.SecondaryEventSelector.InputCollections = [ "SimplePoolFile2.root" ]

svcMgr.DoubleEventSelector.OutputLevel = DEBUG
svcMgr.SecondaryEventSelector.OutputLevel = DEBUG

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
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000
AthenaPoolTestDataReader.OutputLevel = DEBUG

from AthenaServices import AthenaServicesConf
AthenaEventLoopMgr = AthenaServicesConf.AthenaEventLoopMgr()
AthenaEventLoopMgr.UseSecondaryEventNumber = True
svcMgr += AthenaEventLoopMgr

# Turn on the tree cache for the CollectionTree - tree cache only
# works for one tree. And set tree cache size - default is 10 MB (10 000 000)
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; ContainerName = 'CollectionTree'; TREE_CACHE = '100000'" ]
# Set number of events for learning before turning on cache - default is 5
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; TREE_CACHE_LEARN_EVENTS = '6'" ]

# Print out values - must have PoolSvc in info mode
svcMgr.PoolSvc.OutputLevel = INFO
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; TREE_CACHE_LEARN_EVENTS = 'int'" ]
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; TREE_CACHE_SIZE = 'int'" ]

# Print out for each event the number of bytes read and the number of
# read calls. With the tree cache, one should see jumps in the bytes
# read by the cache size, i.e. the bytes read should not change each
# event. However, the cache only works on one tree - the main event
# tree (CollectionTree) - and we read some things from other trees, so
# one does see a small increase event-by-event.
svcMgr.PoolSvc.OutputLevel = INFO
svcMgr.AthenaPoolCnvSvc.PrintInputAttrPerEvt += [ "DatabaseName = '*'; BYTES_READ = 'double'" ]
svcMgr.AthenaPoolCnvSvc.PrintInputAttrPerEvt += [ "DatabaseName = '*'; READ_CALLS = 'int'" ]


# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
