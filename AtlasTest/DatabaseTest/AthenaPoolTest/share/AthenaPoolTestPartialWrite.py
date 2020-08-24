###############################################################
#
# Job options file
#
## @file LArCellContWriter_jobOptions.py
##
## @brief For Athena POOL test: write out LArCellContainers
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
# MC Event Selector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

include( "AthenaPoolTestData/AthenaPoolTestDataDict_joboptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 5
#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "AthenaPoolTestDataWriter" ]
AthenaPoolTestDataWriter = Algorithm( "AthenaPoolTestDataWriter" )
AthenaPoolTestDataWriter.PartialCreate = TRUE
#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# LAr Hit 
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolItemList_joboptions.py" )

print fullItemList

# Stream's output file
Stream1 = Algorithm( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile1.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList
#Stream1.ItemList   += ["EventInfo#*"]
#Stream1.ItemList   += ["IAthenaPoolTestCollection#*"]
#Stream1.ItemList   += ["AthenaPoolTestMatrix#*"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
AthenaPoolTestDataWriter = Algorithm( "AthenaPoolTestDataWriter" )
AthenaPoolTestDataWriter.OutputLevel = 2

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
