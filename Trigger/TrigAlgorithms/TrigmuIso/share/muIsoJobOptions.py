#----------------------------------------------------
#  muIso options
#----------------------------------------------------
# Timing service libs.
theApp.Dlls += [ "TrigTimeAlgs" ]
theApp.ExtSvc += [ "TrigTimeSvc" ]

# Algorithm
theApp.Dlls += [ "TrigmuIso" ]

muIso_Muon_L2 = Algorithm( "muIso_Muon_L2" );
muIso_Muon_L2.OutputLevel=INFO

muIso_900GeV_L2 = Algorithm( "muIso_900GeV_L2" );
muIso_900GeV_L2.OutputLevel=INFO

# Timer service conf.
TrigTimerSvc = Service ( "TrigTimerSvc" )
TrigTimerSvc.OutputLevel = 3
