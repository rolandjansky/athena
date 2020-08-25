###############################################################
#
# Job options file
#
## @file AthenaPoolTestWrite.py
##
## @brief For Athena POOL test: write out AthenaPoolTest data objects 
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
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
# LAr Hit 
# Converters:

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolItemList_joboptions.py" )

printfunc (fullItemList)

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", noTag=True )
Stream1.OutputFile =   "SimplePoolFile.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

# Remove dumm_A from itemlist
#Stream1.ItemList = [ o for o in Stream1.ItemList if not o in 'dummy_A#*' ]
#Stream1.ItemList = [ o for o in Stream1.ItemList if not o in 'AthenaPoolTestMap#*' ]
#Stream1.ItemList = [ o for o in Stream1.ItemList if not o == 'PileUpEventInfo#*'  ]

printfunc (Stream1.ItemList)

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000

#rds svcMgr.ClassIDSvc = Service( "ClassIDSvc" )
#rds svcMgr.ClassIDSvc.OutputLevel = 3

from AthenaServices import AthenaServicesConf
AthenaEventLoopMgr = AthenaServicesConf.AthenaEventLoopMgr()
AthenaEventLoopMgr.OutputLevel = INFO

#svcMgr.StoreGateSvc = Service( "StoreGateSvc" )
#svcMgr.StoreGateSvc.Dump = TRUE 
#svcMgr.StoreGateSvc.OutputLevel = 2

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
