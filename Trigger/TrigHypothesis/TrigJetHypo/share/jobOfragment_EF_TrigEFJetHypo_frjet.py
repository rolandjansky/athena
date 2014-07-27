theApp.Dlls += [ "TrigJetHypo" ]
TrigEFJetHypo_frjet_EF = Algorithm( "TrigEFJetHypo_frjet_EF" )
TrigEFJetHypo_frjet_EF.Etcut = 10*GeV #cut for j10!
TrigEFJetHypo_frjet_EF.AcceptAll = True
