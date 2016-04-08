#----------------------------------------------------
#  muIso options
#----------------------------------------------------
# Timing service libs.
theApp.Dlls += [ "TrigTimeAlgs" ]
theApp.ExtSvc += [ "TrigTimeSvc" ]

# Algorithm
theApp.Dlls += [ "TrigmuIso" ]

#muIso_Muon_L2 = Algorithm( "muIso_Muon_L2" );
#muIso_Muon_L2.EtaWidth=0.35
#muIso_Muon_L2.PhiWidth=0.35
#muIso_Muon_L2.EtaLimit=2.5
#muIso_Muon_L2.EthrECAL=50.0
#muIso_Muon_L2.RinnECAL=0.07
#muIso_Muon_L2.RoutECAL=0.3
#muIso_Muon_L2.EthrECALEndCaps=50.0
#muIso_Muon_L2.RinnECALEndCaps=0.07
#muIso_Muon_L2.RoutECALEndCaps=0.3
#muIso_Muon_L2.EthrHCAL=50.0
#muIso_Muon_L2.RinnHCAL=0.1
#muIso_Muon_L2.RoutHCAL=0.3
#muIso_Muon_L2.EthrHCALEndCaps=50.0
#muIso_Muon_L2.RinnHCALEndCaps=0.1
#muIso_Muon_L2.RoutHCALEndCaps=0.3
#muIso_Muon_L2.UseECAL=True
#muIso_Muon_L2.UseECALEndCaps=True
#muIso_Muon_L2.UseHCAL=True
#muIso_Muon_L2.UseHCALEndCaps=True
#muIso_Muon_L2.UseCombinedMuon=False
#muIso_Muon_L2.OutputLevel=INFO

#muIso_900GeV_L2 = Algorithm( "muIso_900GeV_L2" );
#muIso_900GeV_L2.EtaWidth=0.35
#muIso_900GeV_L2.PhiWidth=0.35
#muIso_900GeV_L2.EtaLimit=2.5
#muIso_900GeV_L2.EthrECAL=50.0
#muIso_900GeV_L2.RinnECAL=0.07
#muIso_900GeV_L2.RoutECAL=0.3
#muIso_900GeV_L2.EthrECALEndCaps=50.0
#muIso_900GeV_L2.RinnECALEndCaps=0.07
#muIso_900GeV_L2.RoutECALEndCaps=0.3
#muIso_900GeV_L2.EthrHCAL=50.0
#muIso_900GeV_L2.RinnHCAL=0.1
#muIso_900GeV_L2.RoutHCAL=0.3
#muIso_900GeV_L2.EthrHCALEndCaps=50.0
#muIso_900GeV_L2.RinnHCALEndCaps=0.1
#muIso_900GeV_L2.RoutHCALEndCaps=0.3
#muIso_900GeV_L2.UseECAL=True
#muIso_900GeV_L2.UseECALEndCaps=True
#muIso_900GeV_L2.UseHCAL=True
#muIso_900GeV_L2.UseHCALEndCaps=True
#muIso_900GeV_L2.UseCombinedMuon=False
#muIso_900GeV_L2.OutputLevel=INFO

# Timer service conf.
TrigTimerSvc = Service ( "TrigTimerSvc" )
TrigTimerSvc.OutputLevel = 3
