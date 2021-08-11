###############################################################
#
# jobOptions file for CaloTTDescr loading an tests
#==============================================================
# unfortunately, need to read dummy events !
# but this one also includes the mandatory one... 
# AthenaCommon/Atlas.UnixStandardJob.txt
#
theApp.setup( MONTECARLO )

theApp.EvtMax = 1
# Set output level threshold 
#(0=ALL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3
include( "CaloConditions/CaloConditions_jobOptions.py" )

theApp.Dlls += [ 
"TestLArDetDescr"
]
theApp.TopAlg += [
"TestLArTT" 
]
