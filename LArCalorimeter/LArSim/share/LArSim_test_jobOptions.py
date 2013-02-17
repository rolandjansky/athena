###############################################################
#
# Job options file
#
# Provides a stand-alone test of LArHits 
#
#==============================================================
theApp.setup( MONTECARLO )

include( "LArSim/LArSim_Fake_jobOptions.py" )

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 2 
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 50 
#==============================================================
#
# End of job options file
#
###############################################################
