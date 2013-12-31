from MissingET.METRefGetter_new import *
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags

if QcdD3PDMakerFlags.doRecJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
    my_alg = make_METRefAlg(_suffix='_GCW_pt20')
    my_alg.jet_JetInputCollectionKey   ='AntiKt6TopoNewJets'
    my_alg.jet_JetPtCut                = 20.0*GeV
    my_alg.jet_ApplyJetScale           = "Yes"
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
    
    my_alg3 = make_METRefAlg(_suffix='_LCW_pt20')
    my_alg3.jet_JetInputCollectionKey   ='AntiKt6LCTopoNewJets'
    my_alg3.jet_JetPtCut                = 20.0*GeV
    my_alg3.jet_ApplyJetScale           = "No"
    my_alg3.jet_UseJetMomentForScale    = True
    my_alg3.jet_JetMomentForScale       = "LCJES"
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
    my_alg3.muon_algorithm              = "Muid"
    my_alg3.muon_isolationAlg           = "dRJet"
    my_alg3()
    
    my_alg4 = make_METRefAlg(_suffix='_LCW_NI_eflow4')
    my_alg4.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
    my_alg4.jet_JetPtCut                = 20.0*GeV
    my_alg4.jet_ApplyJetScale           = "Yes"
    my_alg4.jet_UseJetMomentForScale    = True
    my_alg4.jet_JetMomentForScale       = "LCJES"
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
    my_alg4.muon_algorithm              = "Staco"
    my_alg4.muon_isolationAlg           = "dRJet"
    my_alg4()
    
    my_alg5 = make_METRefAlg(_suffix='_LCW_CB_eflow')
    my_alg5.jet_JetInputCollectionKey   ='AntiKt6LCTopoCBJets'
    my_alg5.jet_JetPtCut                = 20.0*GeV
    my_alg5.jet_ApplyJetScale           = "Yes"
    my_alg5.jet_UseJetMomentForScale    = True
    my_alg5.jet_JetMomentForScale       = "CONST"
    my_alg5.jet_RunSoftJetsTool         = True
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
    my_alg5.muon_isolationAlg           = "dRJet"
    my_alg5()
    
    my_alg6 = make_METRefAlg(_suffix='_LCW_NI_eflow_400')
    my_alg6.jet_JetInputCollectionKey   ='AntiKt6LCTopoNewJets'
    my_alg6.jet_JetPtCut                = 20.0*GeV
    my_alg6.jet_ApplyJetScale           = "Yes"
    my_alg6.jet_UseJetMomentForScale    = True
    my_alg6.jet_JetMomentForScale       = "LCJES"
    my_alg6.jet_RunSoftJetsTool         = True
    my_alg6.jet_SoftJetsPtCut           = 7.0*GeV
    my_alg6.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg6.jet_ApplySoftJetsScale      = "No"
    my_alg6.jet_calibType               ='LocHad'
    my_alg6.ele_calibType               ='RefCalib'
    my_alg6.gamma_calibType             ='EmScale'
    my_alg6.cellout_calibType           ='Eflow'
    my_alg6.cellout_pttrack             = 400
    my_alg6.tau_calibType               ='LocHad'
    my_alg6.cryo_ApplyCorrection        = "Off"
    my_alg6.muon_algorithm              = "Staco"
    my_alg6.muon_isolationAlg           = "dRJet"
    my_alg6()

    my_alg7 = make_METRefAlg(_suffix='_New')
    my_alg7.jet_JetInputCollectionKey   ='AntiKt6LCTopoNewJets'
    my_alg7.jet_JetPtCut                = 20.0*GeV
    my_alg7.jet_ApplyJetScale           = "Yes"
    my_alg7.jet_UseJetMomentForScale    = True
    my_alg7.jet_JetMomentForScale       = "LCJES"
    my_alg7.jet_RunSoftJetsTool         = True
    my_alg7.jet_SoftJetsPtCut           = 7.0*GeV
    my_alg7.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg7.jet_ApplySoftJetsScale      = "No"
    my_alg7.jet_calibType               ='LocHad'
    my_alg7.ele_calibType               ='RefCalib'
    my_alg7.gamma_calibType             ='EmScale'
    my_alg7.cellout_calibType           ='Eflow'
    my_alg7.tau_calibType               ='LocHad'
    my_alg7.cryo_ApplyCorrection        = "Off"
    my_alg7.muon_algorithm              = "Staco"
    my_alg7.muon_isolationAlg           = "dRJet"
    my_alg7()

    my_alg8 = make_METRefAlg(_suffix='_New_em')
    my_alg8.jet_JetInputCollectionKey   ='AntiKt4TopoEMNewJets'
    my_alg8.jet_JetPtCut                = 20.0*GeV
    my_alg8.jet_ApplyJetScale           = "Yes"
    my_alg8.jet_UseJetMomentForScale    = True
    my_alg8.jet_JetMomentForScale       = "EMJES"
    my_alg8.jet_RunSoftJetsTool         = True
    my_alg8.jet_SoftJetsPtCut           = 7.0*GeV
    my_alg8.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg8.jet_ApplySoftJetsScale      = "No"
    my_alg8.jet_calibType               ='EmScale'
    my_alg8.ele_calibType               ='RefCalib'
    my_alg8.gamma_calibType             ='EmScale'
    my_alg8.cellout_calibType           ='EmScale'
    my_alg8.tau_calibType               ='EmScale'
    my_alg8.cryo_ApplyCorrection        = "Off"
    my_alg8.muon_algorithm              = "Muid"
    my_alg8.muon_isolationAlg           = "dRJet"
    my_alg8()

else:
    my_alg = make_METRefAlg(_suffix='_GCW_pt20')
    my_alg.jet_JetInputCollectionKey   ='AntiKt6TopoJets'
    my_alg.jet_JetPtCut                = 20.0*GeV
    my_alg.jet_ApplyJetScale           = "Yes"
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
    
    my_alg3 = make_METRefAlg(_suffix='_LCW_pt20')
    my_alg3.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
    my_alg3.jet_JetPtCut                = 20.0*GeV
    my_alg3.jet_ApplyJetScale           = "No"
    my_alg3.jet_UseJetMomentForScale    = True
    my_alg3.jet_JetMomentForScale       = "LCJES"
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
    my_alg3.muon_algorithm              = "Muid"
    my_alg3.muon_isolationAlg           = "dRJet"
    my_alg3()
    
    my_alg4 = make_METRefAlg(_suffix='_LCW_NI_eflow4')
    my_alg4.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
    my_alg4.jet_JetPtCut                = 20.0*GeV
    my_alg4.jet_ApplyJetScale           = "Yes"
    my_alg4.jet_UseJetMomentForScale    = True
    my_alg4.jet_JetMomentForScale       = "LCJES"
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
    my_alg4.muon_algorithm              = "Staco"
    my_alg4.muon_isolationAlg           = "dRJet"
    my_alg4()
    
    my_alg5 = make_METRefAlg(_suffix='_LCW_CB_eflow')
    my_alg5.jet_JetInputCollectionKey   ='AntiKt6LCTopoCBJets'
    my_alg5.jet_JetPtCut                = 20.0*GeV
    my_alg5.jet_ApplyJetScale           = "Yes"
    my_alg5.jet_UseJetMomentForScale    = True
    my_alg5.jet_JetMomentForScale       = "CONST"
    my_alg5.jet_RunSoftJetsTool         = True
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
    my_alg5.muon_isolationAlg           = "dRJet"
    my_alg5()
    
    my_alg6 = make_METRefAlg(_suffix='_LCW_NI_eflow_400')
    my_alg6.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
    my_alg6.jet_JetPtCut                = 20.0*GeV
    my_alg6.jet_ApplyJetScale           = "Yes"
    my_alg6.jet_UseJetMomentForScale    = True
    my_alg6.jet_JetMomentForScale       = "LCJES"
    my_alg6.jet_RunSoftJetsTool         = True
    my_alg6.jet_SoftJetsPtCut           = 7.0*GeV
    my_alg6.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg6.jet_ApplySoftJetsScale      = "No"
    my_alg6.jet_calibType               ='LocHad'
    my_alg6.ele_calibType               ='RefCalib'
    my_alg6.gamma_calibType             ='EmScale'
    my_alg6.cellout_calibType           ='Eflow'
    my_alg6.cellout_pttrack             = 400
    my_alg6.tau_calibType               ='LocHad'
    my_alg6.cryo_ApplyCorrection        = "Off"
    my_alg6.muon_algorithm              = "Staco"
    my_alg6.muon_isolationAlg           = "dRJet"
    my_alg6()

    my_alg7 = make_METRefAlg(_suffix='_New')
    my_alg7.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
    my_alg7.jet_JetPtCut                = 20.0*GeV
    my_alg7.jet_ApplyJetScale           = "Yes"
    my_alg7.jet_UseJetMomentForScale    = True
    my_alg7.jet_JetMomentForScale       = "LCJES"
    my_alg7.jet_RunSoftJetsTool         = True
    my_alg7.jet_SoftJetsPtCut           = 7.0*GeV
    my_alg7.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg7.jet_ApplySoftJetsScale      = "No"
    my_alg7.jet_calibType               ='LocHad'
    my_alg7.ele_calibType               ='RefCalib'
    my_alg7.gamma_calibType             ='EmScale'
    my_alg7.cellout_calibType           ='Eflow'
    my_alg7.tau_calibType               ='LocHad'
    my_alg7.cryo_ApplyCorrection        = "Off"
    my_alg7.muon_algorithm              = "Staco"
    my_alg7.muon_isolationAlg           = "dRJet"
    my_alg7()

    my_alg8 = make_METRefAlg(_suffix='_New_em')
    my_alg8.jet_JetInputCollectionKey   ='AntiKt4TopoEMJets'
    my_alg8.jet_JetPtCut                = 20.0*GeV
    my_alg8.jet_ApplyJetScale           = "Yes"
    my_alg8.jet_UseJetMomentForScale    = True
    my_alg8.jet_JetMomentForScale       = "EMJES"
    my_alg8.jet_RunSoftJetsTool         = True
    my_alg8.jet_SoftJetsPtCut           = 7.0*GeV
    my_alg8.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg8.jet_ApplySoftJetsScale      = "No"
    my_alg8.jet_calibType               ='EmScale'
    my_alg8.ele_calibType               ='RefCalib'
    my_alg8.gamma_calibType             ='EmScale'
    my_alg8.cellout_calibType           ='EmScale'
    my_alg8.tau_calibType               ='EmScale'
    my_alg8.cryo_ApplyCorrection        = "Off"
    my_alg8.muon_algorithm              = "Muid"
    my_alg8.muon_isolationAlg           = "dRJet"
    my_alg8()
