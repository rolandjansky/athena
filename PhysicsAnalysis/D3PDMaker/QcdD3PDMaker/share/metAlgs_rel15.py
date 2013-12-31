from MissingET.METRefGetter_new import *

my_alg = make_METRefAlg(_suffix='_GCW_pt20')
my_alg.jet_JetInputCollectionKey   ='AntiKt6H1TopoJets'
my_alg.jet_JetPtCut                = 20.0*GeV
my_alg.jet_ApplyJetScale           = "No"
my_alg.jet_UseJetMomentForScale    = True
my_alg.jet_JetMomentForScale       = "GCWJES"
my_alg.jet_RunSoftJetsTool         = True
my_alg.jet_SoftJetsPtCut           = 7.0*GeV
my_alg.jet_SoftJetsMaxPtCut        = 20.0*GeV
my_alg.jet_ApplySoftJetsScale      = "No"
my_alg.jet_calibType               ='H1'
my_alg.ele_calibType               ='RefCalib'
my_alg.gamma_calibType             ='EmScale'
my_alg.cellout_calibType           ='H1'
my_alg.tau_calibType               ='H1'
my_alg.cryo_ApplyCorrection        = "On"
my_alg.muon_algorithm              = "Staco"
my_alg.muon_isolationAlg           = "dRJet"
my_alg()

my_alg2 = make_METRefAlg(_suffix='_GCW_NI_pt20_Muid')
my_alg2.jet_JetInputCollectionKey   ='AntiKt6H1TopoJets'
my_alg2.jet_JetPtCut                = 20.0*GeV
my_alg2.jet_ApplyJetScale           = "Yes"
my_alg2.jet_UseJetMomentForScale    = True
my_alg2.jet_JetMomentForScale       = "GCWJES"
my_alg2.jet_RunSoftJetsTool         = True
my_alg2.jet_SoftJetsPtCut           = 7.0*GeV
my_alg2.jet_SoftJetsMaxPtCut        = 20.0*GeV
my_alg2.jet_ApplySoftJetsScale      = "No"
my_alg2.jet_calibType               ='H1'
my_alg2.ele_calibType               ='RefCalib'
my_alg2.gamma_calibType             ='EmScale'
my_alg2.cellout_calibType           ='H1'
my_alg2.tau_calibType               ='H1'
my_alg2.cryo_ApplyCorrection        = "On"
my_alg2.muon_algorithm              = "Muid"
my_alg2.muon_isolationAlg           = "dRJet"
my_alg2()

my_alg3 = make_METRefAlg(_suffix='_LCW_pt20')
my_alg3.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
my_alg3.jet_JetPtCut                = 20.0*GeV
my_alg3.jet_ApplyJetScale           = "No"
my_alg3.jet_UseJetMomentForScale    = True
my_alg3.jet_JetMomentForScale       = "GCWJES"
my_alg3.jet_RunSoftJetsTool         = True
my_alg3.jet_SoftJetsPtCut           = 7.0*GeV
my_alg3.jet_SoftJetsMaxPtCut        = 20.0*GeV
my_alg3.jet_ApplySoftJetsScale      = "No"
my_alg3.jet_calibType               ='LocHad'
my_alg3.ele_calibType               ='RefCalib'
my_alg3.gamma_calibType             ='EmScale'
my_alg3.cellout_calibType           ='LocHad'
my_alg3.tau_calibType               ='LocHad'
my_alg3.cryo_ApplyCorrection        = "Off"
my_alg3.muon_algorithm              = "Staco"
my_alg3.muon_isolationAlg           = "dRJet"
my_alg3()

my_alg4 = make_METRefAlg(_suffix='_LCW_NI_pt20_Muid_eflow')
my_alg4.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
my_alg4.jet_JetPtCut                = 20.0*GeV
my_alg4.jet_ApplyJetScale           = "Yes"
my_alg4.jet_UseJetMomentForScale    = True
my_alg4.jet_JetMomentForScale       = "GCWJES"
my_alg4.jet_RunSoftJetsTool         = True
my_alg4.jet_SoftJetsPtCut           = 7.0*GeV
my_alg4.jet_SoftJetsMaxPtCut        = 20.0*GeV
my_alg4.jet_ApplySoftJetsScale      = "No"
my_alg4.jet_calibType               ='LocHad'
my_alg4.ele_calibType               ='RefCalib'
my_alg4.gamma_calibType             ='EmScale'
my_alg4.cellout_calibType           ='Eflow'
my_alg4.tau_calibType               ='LocHad'
my_alg4.cryo_ApplyCorrection        = "Off"
my_alg4.muon_algorithm              = "Muid"
my_alg4.muon_isolationAlg           = "dRJet"
my_alg4()

my_alg5 = make_METRefAlg(_suffix='_LCW_NI_pt20_noSoftJets_eflow')
my_alg5.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
my_alg5.jet_JetPtCut                = 20.0*GeV
my_alg5.jet_ApplyJetScale           = "Yes"
my_alg5.jet_UseJetMomentForScale    = True
my_alg5.jet_JetMomentForScale       = "GCWJES"
my_alg5.jet_RunSoftJetsTool         = False
my_alg5.jet_SoftJetsPtCut           = 7.0*GeV
my_alg5.jet_SoftJetsMaxPtCut        = 20.0*GeV
my_alg5.jet_ApplySoftJetsScale      = "No"
my_alg5.jet_calibType               ='LocHad'
my_alg5.ele_calibType               ='RefCalib'
my_alg5.gamma_calibType             ='EmScale'
my_alg5.cellout_calibType           ='Eflow'
my_alg5.tau_calibType               ='LocHad'
my_alg5.cryo_ApplyCorrection        = "Off"
my_alg5.muon_algorithm              = "Staco"
my_alg5.muon_isolationAlg           = "TrkIso"
my_alg5()

my_alg6 = make_METRefAlg(_suffix='_emjes_cellout_em')
my_alg6.jet_JetInputCollectionKey   ='AntiKt6H1TopoEMJets'
my_alg6.jet_JetPtCut                = 20.0*GeV
my_alg6.jet_ApplyJetScale           = "Yes"
my_alg6.jet_UseJetMomentForScale    = True
my_alg6.jet_JetMomentForScale       = "EMJES"
my_alg6.jet_RunSoftJetsTool         = False
my_alg6.jet_SoftJetsPtCut           = 7.0*GeV
my_alg6.jet_SoftJetsMaxPtCut        = 20.0*GeV
my_alg6.jet_ApplySoftJetsScale      = "No"
my_alg6.jet_calibType               ='EmScale'
my_alg6.ele_calibType               ='RefCalib'
my_alg6.gamma_calibType             ='EmScale'
my_alg6.cellout_calibType           ='LocHad'
my_alg6.tau_calibType               ='LocHad'
my_alg6.cryo_ApplyCorrection        = "Off"
my_alg6.muon_algorithm              = "Staco"
my_alg6.muon_isolationAlg           = "dRJet"
my_alg6()
