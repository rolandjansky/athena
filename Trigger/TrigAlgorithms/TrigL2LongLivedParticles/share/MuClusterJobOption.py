#----------------------------------------------------
#  MuCluster options
#----------------------------------------------------
# Timing service libs.
theApp.Dlls += [ "TrigTimeAlgs" ]
theApp.ExtSvc += [ "TrigTimeSvc" ]

# Algorithm
theApp.Dlls += [ "TrigMuCluster" ]


# Timer service conf.
TrigTimerSvc = Service ( "TrigTimerSvc" )
TrigTimerSvc.OutputLevel = 3
