###############################################################
#
# Job options file
#
## @file AthenaPoolTestStep2WriteDoubleSelector.py
##
## @brief For Athena POOL test: write out AthenaPoolTest data objects
## in a second file to test navigation
##
## @author Miha Muskinja <miha.muskinja@cern.ch>
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Set that we are running MC+MC overlay in MT mode
from OverlayCommonAlgs.OverlayFlags import overlayFlags
overlayFlags.isDataOverlay.set_Value_and_Lock(False)
overlayFlags.isOverlayMT.set_Value_and_Lock(True)

#--------------------------------------------------------------
# Load POOL support for DoubleEventSelector
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# Define the input parameters
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
from AthenaPoolTest.AthenaPoolTestConf import AthenaPoolTestDataWriter
topSequence += AthenaPoolTestDataWriter( "AthenaPoolTestDataWriter" )
AthenaPoolTestDataWriter.PartialCreate = TRUE
AthenaPoolTestDataWriter.ReadOtherHalf = TRUE

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolItemList_joboptions.py" )

printfunc (fullItemList)

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream3 = AthenaPoolOutputStream( "Stream3", noTag=True )

# Run OutputStream as an algorithm
topSequence += AthenaPoolOutputStream( "Stream3", noTag=True )
# Must make sure that no OutStream's have been declared
theApp.OutStream     = []; 
# Stream's output file
Stream3.WritingTool = "AthenaOutputStreamTool"
Stream3.OutputFile  = "SimplePoolFile3.root"
# List of DO's to write out
#Stream1.ItemList   += fullItemList
Stream3.ItemList   += ["EventInfo#*"]
# Write out only a new set of cols 1,2,3, and matrix
Stream3.ItemList   += ["IAthenaPoolTestCollection#AthenaPoolTestCollection_1"]
Stream3.ItemList   += ["IAthenaPoolTestCollection#AthenaPoolTestCollection_2"]
Stream3.ItemList   += ["IAthenaPoolTestCollection#AthenaPoolTestCollection_3"]
Stream3.ItemList   += ["AthenaPoolTestMatrix#*"]

printfunc (Stream3.ItemList)

# Change output file catalog to avoid collisions.
from PoolSvc import PoolSvcConf
PoolSvc = PoolSvcConf.PoolSvc()
PoolSvc.WriteCatalog = 'file:AthenaPoolTestStep2WriteDoubleSelector_catalog.xml'


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000
AthenaPoolTestDataWriter.OutputLevel = DEBUG

from AthenaServices import AthenaServicesConf
AthenaEventLoopMgr = AthenaServicesConf.AthenaEventLoopMgr()
AthenaEventLoopMgr.OutputLevel = INFO
AthenaEventLoopMgr.UseSecondaryEventNumber = True
svcMgr += AthenaEventLoopMgr

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
