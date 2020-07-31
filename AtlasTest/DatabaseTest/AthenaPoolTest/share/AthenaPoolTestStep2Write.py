###############################################################
#
# Job options file
#
## @file AthenaPoolTestStep2Write.py
##
## @brief For Athena POOL test: write out AthenaPoolTest data objects
## in a second file to test navigation
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
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

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# Define the input parameters
#--------------------------------------------------------------

svcMgr.EventSelector.InputCollections        = [ "SimplePoolFile.root" ]

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
# Try to also read in the first half that is going to be written out
# again => this should produce an error
#AthenaPoolTestDataWriter.ReadFirstHalf = TRUE
#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolItemList_joboptions.py" )

printfunc (fullItemList)

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", noTag=True )

# Run OutputStream as an algorithm
topSequence += AthenaPoolOutputStream( "Stream1", noTag=True )
# Must make sure that no OutStream's have been declared
theApp.OutStream     = []; 
# Stream's output file
Stream1.WritingTool = "AthenaOutputStreamTool"
Stream1.OutputFile  = "SimplePoolFile1.root"
# List of DO's to write out
#Stream1.ItemList   += fullItemList
Stream1.ItemList   += ["EventInfo#*"]
#Stream1.ItemList   += ["IAthenaPoolTestCollection#*"]
# Write out only a new set of cols 1,2,3, and matrix
Stream1.ItemList   += ["IAthenaPoolTestCollection#AthenaPoolTestCollection_1"]
Stream1.ItemList   += ["IAthenaPoolTestCollection#AthenaPoolTestCollection_2"]
Stream1.ItemList   += ["IAthenaPoolTestCollection#AthenaPoolTestCollection_3"]
Stream1.ItemList   += ["AthenaPoolTestMatrix#*"]

printfunc (Stream1.ItemList)

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000
#rds ClassIDSvc = Service( "ClassIDSvc" )
#rds ClassIDSvc.OutputLevel = 3
AthenaPoolTestDataWriter.OutputLevel = DEBUG

from AthenaServices import AthenaServicesConf
AthenaEventLoopMgr = AthenaServicesConf.AthenaEventLoopMgr()
AthenaEventLoopMgr.OutputLevel = INFO

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
