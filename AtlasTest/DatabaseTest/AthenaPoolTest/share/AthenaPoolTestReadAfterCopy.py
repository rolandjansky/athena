###############################################################
#
# Job options file
#
## @file AthenaPoolTestReadNav.py
##
## @brief For Athena POOL test: read back AthenaPoolTest data objects
## from two files using navigation
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#EventSelector.InputCollection        = "NewPoolTry";
EventSelector = Service( "EventSelector" )
# To read back from two files:
EventSelector.InputCollections       = [ "SimplePoolFile2.root" ]
EventSelector.BackNavigation         = True
#EventSelector.OutputLevel            = DEBUG
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 5
#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "AthenaPoolTestDataReader" ]
#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------
# Dictionaries:
include( "EventInfo/EventInfoDict_joboptions.py" )

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
AthenaPoolTestDataReader = Algorithm( "AthenaPoolTestDataReader" )
AthenaPoolTestDataReader.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
