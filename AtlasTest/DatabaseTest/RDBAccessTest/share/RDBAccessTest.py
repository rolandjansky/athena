#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

include( "RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py" )
theApp.EvtMax = 1

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------

theApp.Dlls     += [ "RDBAccessTest" ]
theApp.TopAlg   += [ "RDBAccessTest" ]

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------
# Dictionaries:


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
#==============================================================
#
# End of job options file
#
###############################################################
