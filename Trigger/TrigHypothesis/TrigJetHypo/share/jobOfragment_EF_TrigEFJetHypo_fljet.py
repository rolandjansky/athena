theApp.Dlls += [ "TrigJetHypo" ]
TrigEFJetHypo_fljet_EF = Algorithm( "TrigEFJetHypo_fljet_EF" )
TrigEFJetHypo_fljet_EF.Etcut = 10*GeV #cut for j10!
TrigEFJetHypo_fljet_EF.AcceptAll = True
