###############################################################
#
# Job options file
#
## @file AthenaPoolTestWriteDoubleSelector.py
##
## @brief For Athena POOL test: write out AthenaPoolTest data objects 
##
## @author Miha Muskinja <miha.muskinja@cern.ch>
#
#==============================================================
# MC Event Selector
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
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# JobOptions for the loading of the AthenaSealSvc
#--------------------------------------------------------------

# Add in dictionary name:
# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = True
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 5

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 0x10000000000
svcMgr.EventSelector.FirstEvent        = 0x100000000
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 2

svcMgr.EventSelector.OutputLevel       = DEBUG

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolTest.AthenaPoolTestConf import AthenaPoolTestDataWriter
topSequence += AthenaPoolTestDataWriter( "AthenaPoolTestDataWriter" )
AthenaPoolTestDataWriter.OutputLevel = DEBUG

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolItemList_joboptions.py" )

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

# Stream1
Stream1 = AthenaPoolOutputStream( "Stream1", noTag=True )
Stream1.OutputFile =   "SimplePoolFile1.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

# Stream2
Stream2 = AthenaPoolOutputStream( "Stream2", noTag=True )
Stream2.OutputFile =   "SimplePoolFile2.root"
# List of DO's to write out
Stream2.ItemList   += fullItemList

# Remove dumm_A from Stream1
Stream1.ItemList = [ o for o in Stream1.ItemList if not o in 'dummy_A#*' ]
Stream1.ItemList = [ o for o in Stream1.ItemList if not o in 'IAthenaPoolTestCollection#*' ]

# Remove dumm_E from Stream2
Stream2.ItemList = [ o for o in Stream2.ItemList if not o in 'dummy_E#*' ]
Stream2.ItemList = [ o for o in Stream2.ItemList if not o in 'AthenaPoolTestMatrix#*' ]

print "Stream1.ItemList ",Stream1.ItemList
print "Stream2.ItemList ",Stream2.ItemList

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000

svcMgr.AthenaSealSvc.OutputLevel = WARNING

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = INFO

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
