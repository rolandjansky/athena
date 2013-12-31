from MissingET.METRefGetter_plup import *
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags

if QcdD3PDMakerFlags.doRecJet():

    my_alg1 = make_METRefAlg(_suffix='_default')
    my_alg1.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
    my_alg1.jet_JetPtCut                = 20.0*GeV
    my_alg1.jet_ApplyJetScale           = "Yes"
    my_alg1.jet_UseJetMomentForScale    = True
    my_alg1.jet_JetMomentForScale       = "LCJES"
    my_alg1.jet_RunSoftJetsTool         = False
    my_alg1.jet_SoftJetsPtCut           = 10.0*GeV
    my_alg1.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg1.jet_ApplySoftJetsScale      = "No"
    my_alg1.jet_calibType               ='LocHad'
    my_alg1.ele_calibType               ='RefCalib'
    my_alg1.gamma_calibType             ='EmScale'
    my_alg1.cellout_calibType           ='Eflow'
    my_alg1.tau_calibType               ='ExclRefCalib'
    my_alg1.cryo_ApplyCorrection        = "Off"
    my_alg1.muon_algorithm              = "Staco"
    my_alg1.muon_isolationAlg           = "dRJet"
    my_alg1()

    my_alg2 = make_METRefAlg(_suffix='_LCW_pt20')
    my_alg2.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
    my_alg2.jet_JetPtCut                = 20.0*GeV
    my_alg2.jet_ApplyJetScale           = "No"
    my_alg2.jet_RunSoftJetsTool         = False
    my_alg2.jet_calibType               ='LocHad'
    my_alg2.ele_isEM                    = PyAthena.egammaPID.ElectronTight
    my_alg2.ele_calibType               ='RefCalib'
    my_alg2.gamma_calibType             ='EmScale'
    my_alg2.cellout_calibType           ='LocHad'
    my_alg2.tau_doTauTool               ='False'
    my_alg2.cryo_ApplyCorrection        = "Off"
    my_alg2.muon_algorithm              = "Muid"
    my_alg2.muon_isolationAlg           = "dRJet"
    my_alg2()
    
    my_alg4 = make_METRefAlg(_suffix='_LCW_NI_eflow6')
    my_alg4.jet_JetInputCollectionKey   ='AntiKt6LCTopoNewJets'
    my_alg4.jet_JetPtCut                = 20.0*GeV
    my_alg4.jet_ApplyJetScale           = "Yes"
    my_alg4.jet_UseJetMomentForScale    = True
    my_alg4.jet_JetMomentForScale       = "LCJES"
    my_alg4.jet_RunSoftJetsTool         = False
    my_alg4.jet_SoftJetsPtCut           = 10.0*GeV
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
    
    if QcdD3PDMakerFlags.doLCTopoCBJet():
        my_alg5 = make_METRefAlg(_suffix='_LCW_CB_eflow')
        my_alg5.jet_JetInputCollectionKey   ='AntiKt4LCTopoCBJets'
        my_alg5.jet_JetPtCut                = 20.0*GeV
        my_alg5.jet_ApplyJetScale           = "Yes"
        my_alg5.jet_UseJetMomentForScale    = True
        my_alg5.jet_JetMomentForScale       = "CONST"
        my_alg5.jet_RunSoftJetsTool         = True
        my_alg5.jet_SoftJetsPtCut           = 10.0*GeV
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
        
    
else:

    my_alg1 = make_METRefAlg(_suffix='_default')
    my_alg1.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
    my_alg1.jet_JetPtCut                = 20.0*GeV
    my_alg1.jet_ApplyJetScale           = "Yes"
    my_alg1.jet_UseJetMomentForScale    = True
    my_alg1.jet_JetMomentForScale       = "LCJES"
    my_alg1.jet_RunSoftJetsTool         = False
    my_alg1.jet_SoftJetsPtCut           = 10.0*GeV
    my_alg1.jet_SoftJetsMaxPtCut        = 20.0*GeV
    my_alg1.jet_ApplySoftJetsScale      = "No"
    my_alg1.jet_calibType               ='LocHad'
    my_alg1.ele_calibType               ='RefCalib'
    my_alg1.gamma_calibType             ='EmScale'
    my_alg1.cellout_calibType           ='Eflow'
    my_alg1.tau_calibType               ='ExclRefCalib'
    my_alg1.cryo_ApplyCorrection        = "Off"
    my_alg1.muon_algorithm              = "Staco"
    my_alg1.muon_isolationAlg           = "dRJet"
    my_alg1()


    my_alg2 = make_METRefAlg(_suffix='_LCW_pt20')
    my_alg2.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
    my_alg2.jet_JetPtCut                = 20.0*GeV
    my_alg2.jet_ApplyJetScale           = "No"
    my_alg2.jet_RunSoftJetsTool         = False
    my_alg2.jet_calibType               ='LocHad'
    my_alg2.ele_isEM                    = PyAthena.egammaPID.ElectronTight
    my_alg2.ele_calibType               ='RefCalib'
    my_alg2.gamma_calibType             ='EmScale'
    my_alg2.cellout_calibType           ='LocHad'
    my_alg2.tau_doTauTool               ='False'
    my_alg2.cryo_ApplyCorrection        = "Off"
    my_alg2.muon_algorithm              = "Muid"
    my_alg2.muon_isolationAlg           = "dRJet"
    my_alg2()
   
    my_alg4 = make_METRefAlg(_suffix='_LCW_NI_eflow6')
    my_alg4.jet_JetInputCollectionKey   ='AntiKt6LCTopoJets'
    my_alg4.jet_JetPtCut                = 20.0*GeV
    my_alg4.jet_ApplyJetScale           = "Yes"
    my_alg4.jet_UseJetMomentForScale    = True
    my_alg4.jet_JetMomentForScale       = "LCJES"
    my_alg4.jet_RunSoftJetsTool         = False
    my_alg4.jet_SoftJetsPtCut           = 10.0*GeV
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
    
    if QcdD3PDMakerFlags.doLCTopoCBJet():
        my_alg5 = make_METRefAlg(_suffix='_LCW_CB_eflow')
        my_alg5.jet_JetInputCollectionKey   ='AntiKt4LCTopoCBJets'
        my_alg5.jet_JetPtCut                = 20.0*GeV
        my_alg5.jet_ApplyJetScale           = "Yes"
        my_alg5.jet_UseJetMomentForScale    = True
        my_alg5.jet_JetMomentForScale       = "CONST"
        my_alg5.jet_RunSoftJetsTool         = True
        my_alg5.jet_SoftJetsPtCut           = 10.0*GeV
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

