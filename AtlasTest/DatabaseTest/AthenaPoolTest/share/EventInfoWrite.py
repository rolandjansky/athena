###############################################################
#
# Job options file - write out tags into EventInfo
#
## @file EventInfoWrite.py
##
## @brief For Athena POOL test: write out new tags into EventInfo
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
# MC Event Selector
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

# Set handler for traceback
#gbl.AthenaServices.SetFatalHandler(438)


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec

# For general flags
rec.doAOD     = False
rec.doTrigger = False
rec.doWriteTAG  = False
DetDescrVersion = "ATLAS-GEO-17-00-00"
include( "RecExCond/RecExCommon_flags.py" )

#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.Calo_setOff()
#DetFlags.detdescr.LAr_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()

# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")
include( "IOVDbSvc/IOVRecExCommon.py" )

#globalflags.ConditionsTag = 'OFLCOND-CSC-00-00-00'
globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-04-00'

if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

# set up all detector description description 
#include ("RecExCond/AllDet_detDescr.py")

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = True

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 0x100000010
svcMgr.EventSelector.FirstEvent        = 0x100000000
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 25

svcMgr.EventSelector.OutputLevel       = DEBUG


#--------------------------------------------------------------
# Application: load TagInfoMgr
#--------------------------------------------------------------
include ( "EventInfoMgt/EventInfoMgt_jobOptions.py" )
#import EventInfoMgt.EventInfoMgtInit

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolTest.AthenaPoolTestConf import EventInfoWriter
topSequence += EventInfoWriter( "EventInfoWriter" )

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )

print fullItemList

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", "SimplePoolFile.root", asAlg = True )
# List of DO's to write out
Stream1.ItemList   += fullItemList

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000
#rds svcMgr.ClassIDSvc = Service( "ClassIDSvc" )
#rds svcMgr.ClassIDSvc.OutputLevel = 3

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = INFO

svcMgr.TagInfoMgr.OutputLevel = DEBUG
#svcMgr.GeoModelSvc.OutputLevel = DEBUG
EventInfoWriter.OutputLevel = DEBUG
# svcMgr.MetaDataStore.Dump = TRUE
from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
ToolSvc += IOVDbMetaDataTool( "IOVDbMetaDataTool" )
IOVDbMetaDataTool.OutputLevel      = INFO


# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
