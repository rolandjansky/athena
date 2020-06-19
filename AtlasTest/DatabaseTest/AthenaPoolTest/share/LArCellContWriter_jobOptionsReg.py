###############################################################
#
# Job options file
#
## @file LArCellContWriter_jobOptionsReg.py
##
## @brief For Athena POOL test: write out LArCellContainers
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
# Set flags and load det descr
#--------------------------------------------------------------
# For general flags
doAOD     = False
doTrigger = False
DetDescrVersion = "ATLAS-CSC-01-02-00"
include( "RecExCond/RecExCommon_flags.py" )

# Set local flags - only need LAr DetDescr
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOff()
      
# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")

# the correct tag should be specified
svcMgr.IOVDbSvc.GlobalTag="OFLCOND-CSC-00-01-00"

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# Define the output Db parameters (the default value are shown)
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber=1
svcMgr.EventSelector.EventsPerRun=5
svcMgr.EventSelector.FirstEvent=1
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
from AthenaPoolTest.AthenaPoolTestConf import LArCellContFakeWriter
topSequence += LArCellContFakeWriter( "LArCellContFakeWriter" )

from AthenaPoolTest.AthenaPoolTestConf import AthenaPoolTestAttrWriter
topSequence += AthenaPoolTestAttrWriter( "AthenaPoolTestAttrWriter" )

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile.root"
# List of DO's to write out
Stream1.ItemList+=["CaloCellContainer#*"]
Stream1.ItemList+=["EventInfo#*"]

# Must make sure that no OutStream's have been declared
theApp.OutStream    = []; 

#--------------------------------------------------------------
# Event Collection Registration
#--------------------------------------------------------------
# Run OutputStream as an algorithm
from RegistrationServices.RegistrationServicesConf import RegistrationStream
RegStream1 = RegistrationStream( "RegStream1" )
RegStream1.CollectionType   = "ExplicitROOT" 
RegStream1.OutputCollection = "NewEventCollection" 
RegStream1.OutputLevel = INFO
# List of DO's to register
RegStream1.ItemList += [ "DataHeader#*" ]
# Key name of AthenaAttributeList used for the tag:
RegStream1.ItemList += [ "AthenaAttributeList#SimpleTag" ]
topSequence += RegStream1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = INFO
#AthenaPoolTestAttrWriter.OutputLevel = 2
LArCellContFakeWriter.OutputLevel = DEBUG
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE 

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
