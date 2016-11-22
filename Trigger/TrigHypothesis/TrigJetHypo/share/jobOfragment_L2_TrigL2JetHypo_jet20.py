theApp.Dlls += [ "TrigJetHypo" ]
#We define 4 instances of the hypo algo to be able to change
#the 
TrigL2JetHypo_jet20a_L2 = Algorithm( "TrigL2JetHypo_jet20a_L2" )
TrigL2JetHypo_jet20a_L2.Etcut_L2 = 20*GeV 
TrigL2JetHypo_jet20a_L2.AcceptAll = True

TrigL2JetHypo_jet20b_L2 = Algorithm( "TrigL2JetHypo_jet20b_L2" )
TrigL2JetHypo_jet20b_L2.Etcut_L2 = 20*GeV 
TrigL2JetHypo_jet20b_L2.AcceptAll = True

TrigL2JetHypo_jet20c_L2 = Algorithm( "TrigL2JetHypo_jet20c_L2" )
TrigL2JetHypo_jet20c_L2.Etcut_L2 = 20*GeV 
TrigL2JetHypo_jet20c_L2.AcceptAll = True

TrigL2JetHypo_jet20d_L2 = Algorithm( "TrigL2JetHypo_jet20d_L2" )
TrigL2JetHypo_jet20d_L2.Etcut_L2 = 20*GeV 
TrigL2JetHypo_jet20d_L2.AcceptAll = True

TrigL2JetHypo_ktjet20_L2 = Algorithm( "TrigL2JetHypo_ktjet20_L2" )
TrigL2JetHypo_ktjet20_L2.Etcut_L2 = 20*GeV 
TrigL2JetHypo_ktjet20_L2.AcceptAll = True
