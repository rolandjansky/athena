#**************************************************************
#
# jopOptions file for Benchmarking using TrigTimerSvc
#
#==============================================================
theApp.Dlls +=["TrigTimeAlgs" ]
theApp.ExtSvc += [ "TrigTimerSvc" ]
# Need Info OutputLevel in order to get end of run statistics
# (0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
TrigTimerSvc = Service( "TrigTimerSvc" )
TrigTimerSvc.OutputLevel  = 3
#
# To control which timers will be active/present use this flag
#
#TrigTimerSvc.IncludeName = "Sequence.+"
#
# To quicly disable one or group
#
#TrigTimerSvc.ExcludeName = ".+T2Calo.+"

