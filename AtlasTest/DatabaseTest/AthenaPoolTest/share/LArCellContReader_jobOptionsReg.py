###############################################################
#
# Job options file
#
## @file LArCellContReader_jobOptionsReg.py
##
## @brief For Athena POOL test: read in LArCellContainers via explicit collections
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
# Define the output Db parameters (the default value are shown)
#--------------------------------------------------------------
#svcMgr.EventSelector.CollectionType  =  "ExplicitROOT" 
svcMgr.EventSelector.InputCollections  = [ "NewEventCollection" ]
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------

from AthenaPoolTest.AthenaPoolTestConf import LArCellContFakeReader
topSequence += LArCellContFakeReader( "LArCellContFakeReader" )

from AthenaPoolTest.AthenaPoolTestConf import AthenaPoolTestAttrReader
topSequence += AthenaPoolTestAttrReader( "AthenaPoolTestAttrReader" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = INFO
svcMgr.MetaDataSvc.OutputLevel = DEBUG
#AthenaPoolTestAttrReader.OutputLevel = DEBUG
LArCellContFakeReader.OutputLevel = DEBUG
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE 

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
