###############################################################
#
# Job options file
#
## @file LArCellContReader_jobOptionsRegStep2.py
##
## @brief For Athena POOL test:  read in athena pool test data
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Define the output Db parameters (the default value are shown)
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.CollectionType    =  "ExplicitROOT" 
EventSelector.InputCollections  = [ "NewEventCollection1" ]
EventSelector.RefName           = "ESD" 

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "AthenaPoolTestDataReader" ]
#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------

# LAr Hit 
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#include( "RecAthenaPool/RecAthenaPool_joboptions.py" )

include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = INFO
#AthenaPoolTestAttrReader = Algorithm ( "AthenaPoolTestAttrReader" )
#AthenaPoolTestAttrReader.OutputLevel = DEBUG
#MissingETBuilderAlg = Algorithm( "MissingETBuilderAlg" )
#MissingETBuilderAlg.OutputLevel = DEBUG
AthenaPoolTestDataReader = Algorithm( "AthenaPoolTestDataReader" )
AthenaPoolTestDataReader.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
