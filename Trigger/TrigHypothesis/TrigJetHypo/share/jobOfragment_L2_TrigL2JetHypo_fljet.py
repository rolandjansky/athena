theApp.Dlls += [ "TrigJetHypo" ]
#We define 4 instances of the hypo algo to be able to change
#the 
TrigL2JetHypo_fljet_L2 = Algorithm( "TrigL2JetHypo_fljet_L2" )
TrigL2JetHypo_fljet_L2.Etcut_L2 = 20*GeV 
TrigL2JetHypo_fljet_L2.AcceptAll = True
