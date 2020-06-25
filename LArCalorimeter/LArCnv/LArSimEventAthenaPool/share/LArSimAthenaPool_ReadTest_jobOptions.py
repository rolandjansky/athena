###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 200
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 2
##include "$DETDESCRDICTIONARYROOT/dict/DetDescrDictionary_joboptions.txt"
##include "$LARSIMEVENTROOT/dict/LArSimEvent_joboptions.txt"
##include "$EVENTINFOROOT/dict/EventInfo_joboptions.txt"
theApp.Dlls   += [ "LArSim" ]
theApp.TopAlg   += [ "LArHitReader" ]
theApp.Dlls   += [ "LArSimEventAthenaPoolPoolCnv" ]
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection        = "LArSimTest.root"
PoolSvc = Service( "PoolSvc" )
#==============================================================
#
# End of job options file
#
###############################################################
