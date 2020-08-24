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
# Application: AthenaPoolTest options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolTest.AthenaPoolTestConf import PileUpEventInfoWriter 
topSequence += PileUpEventInfoWriter( "PileUpEventInfoWriter" )

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# LAr Hit 
# Converters:

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )

print fullItemList

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile.root"

# List of DO's to write out
#Stream1.ItemList   = ["PileUpEventInfo#OverlayEvent"]
#Stream1.ItemList   = ["EventInfo#OverlayEvent"]
#Stream1.ItemList   = ["EventInfo#McEventInfo", "EventInfo#OverlayEvent"]
Stream1.ItemList   += fullItemList

print Stream1.ItemList


#Stream1.ItemList   += ["EventInfo#*"]
#Stream1.ItemList   += ["IAthenaPoolTestCollection#*"]
#Stream1.ItemList   += ["AthenaPoolTestMatrix#*"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000

PileUpEventInfoWriter.OutputLevel = DEBUG

#svcMgr.StoreGateSvc = Service( "StoreGateSvc" )
#svcMgr.StoreGateSvc.Dump = TRUE 

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
#AthenaEventLoopMgr.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
