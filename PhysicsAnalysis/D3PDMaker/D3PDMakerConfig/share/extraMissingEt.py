#-----------------------------------------#
# Script for extra MissingEt calculations #
#-----------------------------------------#


import AthenaPython.PyAthena as PyAthena
PyAthena.load_library('egammaEnumsDict')

import MissingET.METRefGetter_plup

#from MissingET.METRefGetter_plup                  import *

# MissingEt with tight++ electrons and EMJES AntiKt4 jets

#TightPPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_em_tightpp")
#TightPPMissingEtAlg.ele_EleInputCollectionKey   = "ElectronAODCollection"
#TightPPMissingEtAlg.ele_noCut                   = False
#TightPPMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
#TightPPMissingEtAlg.ele_calibType               = "RefCalib"
#TightPPMissingEtAlg.photon_doPhotonTool         = False
#TightPPMissingEtAlg.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
#TightPPMissingEtAlg.gamma_calibType             = "EmScale"
#TightPPMissingEtAlg.tau_doTauTool               = False
#TightPPMissingEtAlg.tau_calibType               = "EmScale"
#TightPPMissingEtAlg.jet_JetInputCollectionKey   = "AntiKt4TopoEMNewJets"
#TightPPMissingEtAlg.jet_JetPtCut                = 20.0*GeV
#TightPPMissingEtAlg.jet_JetMaxPtCut             = 1000000.0*GeV
#TightPPMissingEtAlg.jet_ApplyJetScale           = "Yes"
#TightPPMissingEtAlg.jet_UseJetMomentForScale    = True
#TightPPMissingEtAlg.jet_JetMomentForScale       = "JES"
#TightPPMissingEtAlg.jet_calibType               = "EmScale"
#TightPPMissingEtAlg.jet_RunSoftJetsTool         = True
#TightPPMissingEtAlg.jet_SoftJetsCalibType       = "EmScale"
#TightPPMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
#TightPPMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
#TightPPMissingEtAlg.jet_ApplySoftJetsScale      = "No"
#TightPPMissingEtAlg.cellout_calibType           = "EmScale" 
#TightPPMissingEtAlg.cellout_PUSuppression_JA    = False
#TightPPMissingEtAlg.cryo_ApplyCorrection        = "Off"
#TightPPMissingEtAlg.muon_container              = "MuidMuonCollection"
#TightPPMissingEtAlg.muon_algorithm              = "Muid"
#TightPPMissingEtAlg.muon_isolationAlg           = "dRJet"
#TightPPMissingEtAlg()


# MissingEt with medium++ electrons and EMJES AntiKt4 jets
#MediumPPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_em_mediumpp")
#MediumPPMissingEtAlg.ele_EleInputCollectionKey = "ElectronAODCollection"
#MediumPPMissingEtAlg.ele_noCut                 = False
#MediumPPMissingEtAlg.ele_isEM                  = PyAthena.egammaPID.ElectronIDMediumPP
#MediumPPMissingEtAlg.ele_calibType             = "RefCalib"
#MediumPPMissingEtAlg.photon_doPhotonTool       = False
#MediumPPMissingEtAlg.gamma_isEM                = PyAthena.egammaPID.PhotonIDTight
#MediumPPMissingEtAlg.gamma_calibType           = "EmScale"
#MediumPPMissingEtAlg.tau_doTauTool             = False
#MediumPPMissingEtAlg.tau_calibType             = "EmScale"
#MediumPPMissingEtAlg.jet_JetInputCollectionKey = "AntiKt4TopoEMNewJets"
#MediumPPMissingEtAlg.jet_JetPtCut              = 20.0*GeV
#MediumPPMissingEtAlg.jet_JetMaxPtCut           = 1000000.0*GeV
#MediumPPMissingEtAlg.jet_ApplyJetScale         = "Yes"
#MediumPPMissingEtAlg.jet_UseJetMomentForScale  = True
#MediumPPMissingEtAlg.jet_JetMomentForScale     = "JES"
#MediumPPMissingEtAlg.jet_calibType             = "EmScale"
#MediumPPMissingEtAlg.jet_RunSoftJetsTool       = True
#MediumPPMissingEtAlg.jet_SoftJetsCalibType     = "EmScale"
#MediumPPMissingEtAlg.jet_SoftJetsPtCut         = 10.0*GeV
#MediumPPMissingEtAlg.jet_SoftJetsMaxPtCut      = 20.0*GeV
#MediumPPMissingEtAlg.jet_ApplySoftJetsScale    = "No"
#MediumPPMissingEtAlg.cellout_calibType         = "EmScale" 
#MediumPPMissingEtAlg.cellout_PUSuppression_JA  = False
#MediumPPMissingEtAlg.cryo_ApplyCorrection      = "Off"
#MediumPPMissingEtAlg.muon_container            = "MuidMuonCollection"
#MediumPPMissingEtAlg.muon_algorithm            = "Muid"
#MediumPPMissingEtAlg.muon_isolationAlg         = "dRJet"
#MediumPPMissingEtAlg()


# MissingEt with loose++ electrons and EMJES AntiKt4 jets

#LoosePPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_em_loosepp")
#LoosePPMissingEtAlg.ele_EleInputCollectionKey = "ElectronAODCollection"
#LoosePPMissingEtAlg.ele_noCut                 = False
#LoosePPMissingEtAlg.ele_isEM                  = PyAthena.egammaPID.ElectronIDLoosePP
#LoosePPMissingEtAlg.ele_calibType             = "RefCalib"
#LoosePPMissingEtAlg.photon_doPhotonTool       = False
#LoosePPMissingEtAlg.gamma_isEM                = PyAthena.egammaPID.PhotonIDTight
#LoosePPMissingEtAlg.gamma_calibType           = "EmScale"
#LoosePPMissingEtAlg.tau_doTauTool             = False
#LoosePPMissingEtAlg.tau_calibType             = "EmScale"
#LoosePPMissingEtAlg.jet_JetInputCollectionKey = "AntiKt4TopoEMNewJets"
#LoosePPMissingEtAlg.jet_JetPtCut              = 20.0*GeV
#LoosePPMissingEtAlg.jet_JetMaxPtCut           = 1000000.0*GeV
#LoosePPMissingEtAlg.jet_ApplyJetScale         = "Yes"
#LoosePPMissingEtAlg.jet_UseJetMomentForScale  = True
#LoosePPMissingEtAlg.jet_JetMomentForScale     = "JES"
#LoosePPMissingEtAlg.jet_calibType             = "EmScale"
#LoosePPMissingEtAlg.jet_RunSoftJetsTool       = True
#LoosePPMissingEtAlg.jet_SoftJetsCalibType     = "EmScale"
#LoosePPMissingEtAlg.jet_SoftJetsPtCut         = 10.0*GeV
#LoosePPMissingEtAlg.jet_SoftJetsMaxPtCut      = 20.0*GeV
#LoosePPMissingEtAlg.jet_ApplySoftJetsScale    = "No"
#LoosePPMissingEtAlg.cellout_calibType         = "EmScale" 
#LoosePPMissingEtAlg.cellout_PUSuppression_JA  = False
#LoosePPMissingEtAlg.cryo_ApplyCorrection      = "Off"
#LoosePPMissingEtAlg.muon_container            = "MuidMuonCollection"
#LoosePPMissingEtAlg.muon_algorithm            = "Muid"
#LoosePPMissingEtAlg.muon_isolationAlg         = "dRJet"
#LoosePPMissingEtAlg()

# MissingEt with tight++ electrons and AntiKt4LCTopoNewJets jets
LCJetTightPPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_tightpp")
LCJetTightPPMissingEtAlg.ele_EleInputCollectionKey   = "ElectronAODCollection"
LCJetTightPPMissingEtAlg.ele_noCut                   = False
LCJetTightPPMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
LCJetTightPPMissingEtAlg.ele_calibType               = "RefCalib"
LCJetTightPPMissingEtAlg.photon_doPhotonTool         = False
LCJetTightPPMissingEtAlg.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
LCJetTightPPMissingEtAlg.gamma_calibType             = "EmScale"
LCJetTightPPMissingEtAlg.tau_doTauTool               = False
LCJetTightPPMissingEtAlg.tau_calibType               = "EmScale"
LCJetTightPPMissingEtAlg.jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
LCJetTightPPMissingEtAlg.jet_JetPtCut                = 20.0*GeV
LCJetTightPPMissingEtAlg.jet_JetMaxPtCut             = 1000000.0*GeV
LCJetTightPPMissingEtAlg.jet_ApplyJetScale           = "Yes"
LCJetTightPPMissingEtAlg.jet_UseJetMomentForScale    = True
LCJetTightPPMissingEtAlg.jet_JetMomentForScale       = "JES"
LCJetTightPPMissingEtAlg.jet_calibType               = "LocHad"
LCJetTightPPMissingEtAlg.jet_RunSoftJetsTool         = True
LCJetTightPPMissingEtAlg.jet_SoftJetsCalibType       = "LocHad"
LCJetTightPPMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
LCJetTightPPMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
LCJetTightPPMissingEtAlg.jet_ApplySoftJetsScale      = "No"
LCJetTightPPMissingEtAlg.cellout_PUSuppression_JA    = False
LCJetTightPPMissingEtAlg.cryo_ApplyCorrection        = "Off"
LCJetTightPPMissingEtAlg.muon_container              = "MuidMuonCollection"
LCJetTightPPMissingEtAlg.muon_algorithm              = "Muid"
LCJetTightPPMissingEtAlg.muon_isolationAlg           = "dRJet"
LCJetTightPPMissingEtAlg()

# MissingEt with tight++ electrons and AntiKt4LCTopoNewJets jets
LC5JetTightPPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_tightpp_AntiKt5LCTopo")
LC5JetTightPPMissingEtAlg.ele_EleInputCollectionKey   = "ElectronAODCollection"
LC5JetTightPPMissingEtAlg.ele_noCut                   = False
LC5JetTightPPMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
LC5JetTightPPMissingEtAlg.ele_calibType               = "RefCalib"
LC5JetTightPPMissingEtAlg.photon_doPhotonTool         = False
LC5JetTightPPMissingEtAlg.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
LC5JetTightPPMissingEtAlg.gamma_calibType             = "EmScale"
LC5JetTightPPMissingEtAlg.tau_doTauTool               = False
LC5JetTightPPMissingEtAlg.tau_calibType               = "EmScale"
LC5JetTightPPMissingEtAlg.jet_JetInputCollectionKey   = "AntiKt5LCTopoNewJets"
LC5JetTightPPMissingEtAlg.jet_JetPtCut                = 20.0*GeV
LC5JetTightPPMissingEtAlg.jet_JetMaxPtCut             = 1000000.0*GeV
LC5JetTightPPMissingEtAlg.jet_ApplyJetScale           = "Yes"
LC5JetTightPPMissingEtAlg.jet_UseJetMomentForScale    = True
#LC5JetTightPPMissingEtAlg.jet_JetMomentForScale       = "JES"
LC5JetTightPPMissingEtAlg.jet_calibType               = "LocHad"
LC5JetTightPPMissingEtAlg.jet_RunSoftJetsTool         = True
LC5JetTightPPMissingEtAlg.jet_SoftJetsCalibType       = "LocHad"
LC5JetTightPPMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
LC5JetTightPPMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
LC5JetTightPPMissingEtAlg.jet_ApplySoftJetsScale      = "No"
LC5JetTightPPMissingEtAlg.cellout_PUSuppression_JA    = False
LC5JetTightPPMissingEtAlg.cryo_ApplyCorrection        = "Off"
LC5JetTightPPMissingEtAlg.muon_container              = "MuidMuonCollection"
LC5JetTightPPMissingEtAlg.muon_algorithm              = "Muid"
LC5JetTightPPMissingEtAlg.muon_isolationAlg           = "dRJet"
LC5JetTightPPMissingEtAlg()


# MissingEt with medium++ electrons and AntiKt4LCTopoNewJets jets
LCJetMediumPPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_mediumpp")
LCJetMediumPPMissingEtAlg.ele_EleInputCollectionKey = "ElectronAODCollection"
LCJetMediumPPMissingEtAlg.ele_noCut                 = False
LCJetMediumPPMissingEtAlg.ele_isEM                  = PyAthena.egammaPID.ElectronIDMediumPP
LCJetMediumPPMissingEtAlg.ele_calibType             = "RefCalib"
LCJetMediumPPMissingEtAlg.photon_doPhotonTool       = False
LCJetMediumPPMissingEtAlg.gamma_isEM                = PyAthena.egammaPID.PhotonIDTight
LCJetMediumPPMissingEtAlg.gamma_calibType           = "EmScale"
LCJetMediumPPMissingEtAlg.tau_doTauTool             = False
LCJetMediumPPMissingEtAlg.tau_calibType             = "EmScale"
LCJetMediumPPMissingEtAlg.jet_JetInputCollectionKey = "AntiKt4LCTopoNewJets"
LCJetMediumPPMissingEtAlg.jet_JetPtCut              = 20.0*GeV
LCJetMediumPPMissingEtAlg.jet_JetMaxPtCut           = 1000000.0*GeV
LCJetMediumPPMissingEtAlg.jet_ApplyJetScale         = "Yes"
LCJetMediumPPMissingEtAlg.jet_UseJetMomentForScale  = True
LCJetMediumPPMissingEtAlg.jet_JetMomentForScale     = "JES"
LCJetMediumPPMissingEtAlg.jet_calibType             = "LocHad"
LCJetMediumPPMissingEtAlg.jet_RunSoftJetsTool       = True
LCJetMediumPPMissingEtAlg.jet_SoftJetsCalibType     = "LocHad"
LCJetMediumPPMissingEtAlg.jet_SoftJetsPtCut         = 10.0*GeV
LCJetMediumPPMissingEtAlg.jet_SoftJetsMaxPtCut      = 20.0*GeV
LCJetMediumPPMissingEtAlg.jet_ApplySoftJetsScale    = "No"
LCJetMediumPPMissingEtAlg.cellout_PUSuppression_JA  = False
LCJetMediumPPMissingEtAlg.cryo_ApplyCorrection      = "Off"
LCJetMediumPPMissingEtAlg.muon_container            = "MuidMuonCollection"
LCJetMediumPPMissingEtAlg.muon_algorithm            = "Muid"
LCJetMediumPPMissingEtAlg.muon_isolationAlg         = "dRJet"
LCJetMediumPPMissingEtAlg()

# MissingEt with medium++ electrons and AntiKt5LCTopoNewJets jets
LC5JetMediumPPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_mediumpp_AntiKt5LCTopo")
LC5JetMediumPPMissingEtAlg.ele_EleInputCollectionKey = "ElectronAODCollection"
LC5JetMediumPPMissingEtAlg.ele_noCut                 = False
LC5JetMediumPPMissingEtAlg.ele_isEM                  = PyAthena.egammaPID.ElectronIDMediumPP
LC5JetMediumPPMissingEtAlg.ele_calibType             = "RefCalib"
LC5JetMediumPPMissingEtAlg.photon_doPhotonTool       = False
LC5JetMediumPPMissingEtAlg.gamma_isEM                = PyAthena.egammaPID.PhotonIDTight
LC5JetMediumPPMissingEtAlg.gamma_calibType           = "EmScale"
LC5JetMediumPPMissingEtAlg.tau_doTauTool             = False
LC5JetMediumPPMissingEtAlg.tau_calibType             = "EmScale"
LC5JetMediumPPMissingEtAlg.jet_JetInputCollectionKey = "AntiKt5LCTopoNewJets"
LC5JetMediumPPMissingEtAlg.jet_JetPtCut              = 20.0*GeV
LC5JetMediumPPMissingEtAlg.jet_JetMaxPtCut           = 1000000.0*GeV
LC5JetMediumPPMissingEtAlg.jet_ApplyJetScale         = "Yes"
LC5JetMediumPPMissingEtAlg.jet_UseJetMomentForScale  = True
#LC5JetMediumPPMissingEtAlg.jet_JetMomentForScale     = "JES"
LC5JetMediumPPMissingEtAlg.jet_calibType             = "LocHad"
LC5JetMediumPPMissingEtAlg.jet_RunSoftJetsTool       = True
LC5JetMediumPPMissingEtAlg.jet_SoftJetsCalibType     = "LocHad"
LC5JetMediumPPMissingEtAlg.jet_SoftJetsPtCut         = 10.0*GeV
LC5JetMediumPPMissingEtAlg.jet_SoftJetsMaxPtCut      = 20.0*GeV
LC5JetMediumPPMissingEtAlg.jet_ApplySoftJetsScale    = "No"
LC5JetMediumPPMissingEtAlg.cellout_PUSuppression_JA  = False
LC5JetMediumPPMissingEtAlg.cryo_ApplyCorrection      = "Off"
LC5JetMediumPPMissingEtAlg.muon_container            = "MuidMuonCollection"
LC5JetMediumPPMissingEtAlg.muon_algorithm            = "Muid"
LC5JetMediumPPMissingEtAlg.muon_isolationAlg         = "dRJet"
LC5JetMediumPPMissingEtAlg()


# MissingEt with loose++ electrons and AntiKt4LCTopoNewJets jets
#LCJetLoosePPMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_loosepp")
#LCJetLoosePPMissingEtAlg.ele_EleInputCollectionKey = "ElectronAODCollection"
#LCJetLoosePPMissingEtAlg.ele_noCut                 = False
#LCJetLoosePPMissingEtAlg.ele_isEM                  = PyAthena.egammaPID.ElectronIDLoosePP
#LCJetLoosePPMissingEtAlg.ele_calibType             = "RefCalib"
#LCJetLoosePPMissingEtAlg.photon_doPhotonTool       = False
#LCJetLoosePPMissingEtAlg.gamma_isEM                = PyAthena.egammaPID.PhotonIDTight
#LCJetLoosePPMissingEtAlg.gamma_calibType           = "EmScale"
#LCJetLoosePPMissingEtAlg.tau_doTauTool             = False
#LCJetLoosePPMissingEtAlg.tau_calibType             = "EmScale"
#LCJetLoosePPMissingEtAlg.jet_JetInputCollectionKey = "AntiKt4LCTopoNewJets"
#LCJetLoosePPMissingEtAlg.jet_JetPtCut              = 20.0*GeV
#LCJetLoosePPMissingEtAlg.jet_JetMaxPtCut           = 1000000.0*GeV
#LCJetLoosePPMissingEtAlg.jet_ApplyJetScale         = "Yes"
#LCJetLoosePPMissingEtAlg.jet_UseJetMomentForScale  = True
#LCJetLoosePPMissingEtAlg.jet_JetMomentForScale     = "JES"
#LCJetLoosePPMissingEtAlg.jet_calibType             = "LocHad"
#LCJetLoosePPMissingEtAlg.jet_RunSoftJetsTool       = True
#LCJetLoosePPMissingEtAlg.jet_SoftJetsCalibType     = "LocHad"
#LCJetLoosePPMissingEtAlg.jet_SoftJetsPtCut         = 10.0*GeV
#LCJetLoosePPMissingEtAlg.jet_SoftJetsMaxPtCut      = 20.0*GeV
#LCJetLoosePPMissingEtAlg.jet_ApplySoftJetsScale    = "No"
#LCJetLoosePPMissingEtAlg.cellout_PUSuppression_JA  = False
#LCJetLoosePPMissingEtAlg.cryo_ApplyCorrection      = "Off"
#LCJetLoosePPMissingEtAlg.muon_container            = "MuidMuonCollection"
#LCJetLoosePPMissingEtAlg.muon_algorithm            = "Muid"
#LCJetLoosePPMissingEtAlg.muon_isolationAlg         = "dRJet"
#LCJetLoosePPMissingEtAlg()

# MissingEt with tight++ electrons, tight photons and AntiKt4LCTopoNewJets jets

#LCJetTightPPPhotonTightMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_tightpp_photonTightAR")

#LCJetTightPPPhotonTightMissingEtAlg.ele_EleInputCollectionKey   = "ElectronAODCollection"
#LCJetTightPPPhotonTightMissingEtAlg.ele_noCut                   = False
#LCJetTightPPPhotonTightMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
#LCJetTightPPPhotonTightMissingEtAlg.ele_calibType               = "RefCalib"
#LCJetTightPPPhotonTightMissingEtAlg.photon_doPhotonTool         = True
#LCJetTightPPPhotonTightMissingEtAlg.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTightAR
#LCJetTightPPPhotonTightMissingEtAlg.gamma_calibType             = "EmScale"
#LCJetTightPPPhotonTightMissingEtAlg.tau_doTauTool               = False
#LCJetTightPPPhotonTightMissingEtAlg.tau_calibType               = "EmScale"
#LCJetTightPPPhotonTightMissingEtAlg.jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
#LCJetTightPPPhotonTightMissingEtAlg.jet_JetPtCut                = 20.0*GeV
#LCJetTightPPPhotonTightMissingEtAlg.jet_JetMaxPtCut             = 1000000.0*GeV
#LCJetTightPPPhotonTightMissingEtAlg.jet_ApplyJetScale           = "Yes"
#LCJetTightPPPhotonTightMissingEtAlg.jet_UseJetMomentForScale    = True
#LCJetTightPPPhotonTightMissingEtAlg.jet_JetMomentForScale       = "JES"
#LCJetTightPPPhotonTightMissingEtAlg.jet_calibType               = "LocHad"
#LCJetTightPPPhotonTightMissingEtAlg.jet_RunSoftJetsTool         = True
#LCJetTightPPPhotonTightMissingEtAlg.jet_SoftJetsCalibType       = "LocHad"
#LCJetTightPPPhotonTightMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
#LCJetTightPPPhotonTightMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
#LCJetTightPPPhotonTightMissingEtAlg.jet_ApplySoftJetsScale      = "No"
#LCJetTightPPPhotonTightMissingEtAlg.cellout_PUSuppression_JA    = False
#LCJetTightPPPhotonTightMissingEtAlg.cryo_ApplyCorrection        = "Off"
#LCJetTightPPPhotonTightMissingEtAlg.muon_container              = "MuidMuonCollection"
#LCJetTightPPPhotonTightMissingEtAlg.muon_algorithm              = "Muid"
#LCJetTightPPPhotonTightMissingEtAlg.muon_isolationAlg           = "dRJet"
#LCJetTightPPPhotonTightMissingEtAlg()

# MissingEt with medium++ electrons, tight photons and AntiKt4LCTopoNewJets jets

#LCJetMediumPPPhotonTightMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_mediumpp_photonTightAR")
#LCJetMediumPPPhotonTightMissingEtAlg.ele_EleInputCollectionKey   = "ElectronAODCollection"
#LCJetMediumPPPhotonTightMissingEtAlg.ele_noCut                   = False
#LCJetMediumPPPhotonTightMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDMediumPP
#LCJetMediumPPPhotonTightMissingEtAlg.ele_calibType               = "RefCalib"
#LCJetMediumPPPhotonTightMissingEtAlg.photon_doPhotonTool         = True
#LCJetMediumPPPhotonTightMissingEtAlg.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTightAR
#LCJetMediumPPPhotonTightMissingEtAlg.gamma_calibType             = "EmScale"
#LCJetMediumPPPhotonTightMissingEtAlg.tau_doTauTool               = False
#LCJetMediumPPPhotonTightMissingEtAlg.tau_calibType               = "EmScale"
#LCJetMediumPPPhotonTightMissingEtAlg.jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
#LCJetMediumPPPhotonTightMissingEtAlg.jet_JetPtCut                = 20.0*GeV
#LCJetMediumPPPhotonTightMissingEtAlg.jet_JetMaxPtCut             = 1000000.0*GeV
#LCJetMediumPPPhotonTightMissingEtAlg.jet_ApplyJetScale           = "Yes"
#LCJetMediumPPPhotonTightMissingEtAlg.jet_UseJetMomentForScale    = True
#LCJetMediumPPPhotonTightMissingEtAlg.jet_JetMomentForScale       = "JES"
#LCJetMediumPPPhotonTightMissingEtAlg.jet_calibType               = "LocHad"
#LCJetMediumPPPhotonTightMissingEtAlg.jet_RunSoftJetsTool         = True
#LCJetMediumPPPhotonTightMissingEtAlg.jet_SoftJetsCalibType       = "LocHad"
#LCJetMediumPPPhotonTightMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
#LCJetMediumPPPhotonTightMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
#LCJetMediumPPPhotonTightMissingEtAlg.jet_ApplySoftJetsScale      = "No"
#LCJetMediumPPPhotonTightMissingEtAlg.cellout_PUSuppression_JA    = False
#LCJetMediumPPPhotonTightMissingEtAlg.cryo_ApplyCorrection        = "Off"
#LCJetMediumPPPhotonTightMissingEtAlg.muon_container              = "MuidMuonCollection"
#LCJetMediumPPPhotonTightMissingEtAlg.muon_algorithm              = "Muid"
#LCJetMediumPPPhotonTightMissingEtAlg.muon_isolationAlg           = "dRJet"
#LCJetMediumPPPhotonTightMissingEtAlg()

# MissingEt with tight++ electrons, loose photons and AntiKt4LCTopoNewJets jets

#LCJetTightPPPhotonLooseMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_tightpp_photonLooseAR")
#LCJetTightPPPhotonLooseMissingEtAlg.ele_EleInputCollectionKey   = "ElectronAODCollection"
#LCJetTightPPPhotonLooseMissingEtAlg.ele_noCut                   = False
#LCJetTightPPPhotonLooseMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
#LCJetTightPPPhotonLooseMissingEtAlg.ele_calibType               = "RefCalib"
#LCJetTightPPPhotonLooseMissingEtAlg.photon_doPhotonTool         = True
#LCJetTightPPPhotonLooseMissingEtAlg.gamma_isEM                  = PyAthena.egammaPID.PhotonIDLooseAR
#LCJetTightPPPhotonLooseMissingEtAlg.gamma_calibType             = "EmScale"
#LCJetTightPPPhotonLooseMissingEtAlg.tau_doTauTool               = False
#LCJetTightPPPhotonLooseMissingEtAlg.tau_calibType               = "EmScale"
#LCJetTightPPPhotonLooseMissingEtAlg.jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
#LCJetTightPPPhotonLooseMissingEtAlg.jet_JetPtCut                = 20.0*GeV
#LCJetTightPPPhotonLooseMissingEtAlg.jet_JetMaxPtCut             = 1000000.0*GeV
#LCJetTightPPPhotonLooseMissingEtAlg.jet_ApplyJetScale           = "Yes"
#LCJetTightPPPhotonLooseMissingEtAlg.jet_UseJetMomentForScale    = True
#LCJetTightPPPhotonLooseMissingEtAlg.jet_JetMomentForScale       = "JES"
#LCJetTightPPPhotonLooseMissingEtAlg.jet_calibType               = "LocHad"
#LCJetTightPPPhotonLooseMissingEtAlg.jet_RunSoftJetsTool         = True
#LCJetTightPPPhotonLooseMissingEtAlg.jet_SoftJetsCalibType       = "LocHad"
#LCJetTightPPPhotonLooseMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
#LCJetTightPPPhotonLooseMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
#LCJetTightPPPhotonLooseMissingEtAlg.jet_ApplySoftJetsScale      = "No"
#LCJetTightPPPhotonLooseMissingEtAlg.cellout_PUSuppression_JA    = False
#LCJetTightPPPhotonLooseMissingEtAlg.cryo_ApplyCorrection        = "Off"
#LCJetTightPPPhotonLooseMissingEtAlg.muon_container              = "MuidMuonCollection"
#LCJetTightPPPhotonLooseMissingEtAlg.muon_algorithm              = "Muid"
#LCJetTightPPPhotonLooseMissingEtAlg.muon_isolationAlg           = "dRJet"
#LCJetTightPPPhotonLooseMissingEtAlg()

#calculate new MissEt Final for JetElectron (same configuration as tight met in topinputs, but different Jet settings!!!)

#from QCDTemplateTool.QCDTemplateToolConf import JetElectronSelector
#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()
#topSequence += JetElectronSelector( "JetElectronSelectorInstance90" )
#topSequence.JetElectronSelectorInstance90.OutputLevel=5 #otherwise we get INFO: Execute() each for each event
#topSequence.JetElectronSelectorInstance90.InputJetContainer="AntiKt4TopoEMNewJets"
#topSequence.JetElectronSelectorInstance90.OutputJetElectronContainer="JetElectronContainer90"
#topSequence.JetElectronSelectorInstance90.OutputJetMETContainer="JetMETContainer90"
#topSequence.JetElectronSelectorInstance90.OutputJetContainer="JetMinusJetElectronContainer90"
#topSequence.JetElectronSelectorInstance90.ExcludeCrackRegion=True
#topSequence.JetElectronSelectorInstance90.CalibrationFactor=0.90

#TightPPMissingEtAlg_jetele = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_em_jetelepp")

#TightPPMissingEtAlg_jetele.ele_EleInputCollectionKey   = "ElectronAODCollection"
#TightPPMissingEtAlg_jetele.ele_noCut                   = False
#TightPPMissingEtAlg_jetele.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
#TightPPMissingEtAlg_jetele.ele_calibType               = "RefCalib"
#TightPPMissingEtAlg_jetele.photon_doPhotonTool         = False
#TightPPMissingEtAlg_jetele.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
#TightPPMissingEtAlg_jetele.gamma_calibType             = "EmScale"
#TightPPMissingEtAlg_jetele.tau_doTauTool               = False
#TightPPMissingEtAlg_jetele.tau_calibType               = "EmScale"
#TightPPMissingEtAlg_jetele.jet_JetInputCollectionKey   = "JetMETContainer90"
#TightPPMissingEtAlg_jetele.jet_JetPtCut                = 20.0*GeV
#TightPPMissingEtAlg_jetele.jet_JetMaxPtCut             = 1000000.0*GeV
#TightPPMissingEtAlg_jetele.jet_ApplyJetScale           = "Yes"
#TightPPMissingEtAlg_jetele.jet_UseJetMomentForScale    = True
#TightPPMissingEtAlg_jetele.jet_JetMomentForScale       = "JetElectronCalibration"
#TightPPMissingEtAlg_jetele.jet_calibType               = "EmScale"
#TightPPMissingEtAlg_jetele.jet_RunSoftJetsTool         = True
#TightPPMissingEtAlg_jetele.jet_SoftJetsCalibType       = "EmScale"
#TightPPMissingEtAlg_jetele.jet_SoftJetsPtCut           = 10.0*GeV
#TightPPMissingEtAlg_jetele.jet_SoftJetsMaxPtCut        = 20.0*GeV
#TightPPMissingEtAlg_jetele.jet_ApplySoftJetsScale      = "No"
#TightPPMissingEtAlg_jetele.cellout_calibType           = "EmScale" 
#TightPPMissingEtAlg_jetele.cellout_PUSuppression_JA    = False
#TightPPMissingEtAlg_jetele.cryo_ApplyCorrection        = "Off"
#TightPPMissingEtAlg_jetele.muon_container              = "MuidMuonCollection"
#TightPPMissingEtAlg_jetele.muon_algorithm              = "Muid"
#TightPPMissingEtAlg_jetele.muon_isolationAlg           = "dRJet"
#TightPPMissingEtAlg_jetele()

# default STVF MET with tightpp electrons and muid muons
recAlgs.doMissingET.set_Value_and_Lock(True)
import MissingET.METRefGetter_plup
#STVFMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_STVF_top")
#STVFMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
#STVFMissingEtAlg.jet_JetInputCollectionKey   =  "AntiKt4LCTopoNewJets"
#STVFMissingEtAlg.photon_doPhotonTool         = False
#STVFMissingEtAlg.tau_doTauTool               = False
#STVFMissingEtAlg.jet_ApplyJetJVF             = "Yes"
#STVFMissingEtAlg.jet_RunSoftJetsTool         = False
#STVFMissingEtAlg.jet_calibType               = "ExclRefCalib"
#STVFMissingEtAlg.muon_container              = "MuidMuonCollection"
#STVFMissingEtAlg.muon_algorithm              = "Muid"
#STVFMissingEtAlg.plupSuppCorr                = 'STVF'
#STVFMissingEtAlg.celloutCorrection           = 'STVF'
#STVFMissingEtAlg.cellout_PUSuppression_JA    =  False
#STVFMissingEtAlg()

# default MissingEt with tight++ electrons, muid muons, and ExclRefCalib jet calibration

#LCJetTightPPERCMissingEtAlg = MissingET.METRefGetter_plup.make_METRefAlg(_suffix = "_tightpp_ExclRefCalib")
#LCJetTightPPERCMissingEtAlg.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
#LCJetTightPPERCMissingEtAlg.jet_JetPtCut                = 20.0*GeV
#LCJetTightPPERCMissingEtAlg.jet_ApplyJetScale           = "Yes"
#LCJetTightPPERCMissingEtAlg.jet_UseJetMomentForScale    = True
#LCJetTightPPERCMissingEtAlg.jet_JetMomentForScale       = "JES"
#LCJetTightPPERCMissingEtAlg.jet_RunSoftJetsTool         = False
#LCJetTightPPERCMissingEtAlg.jet_SoftJetsPtCut           = 10.0*GeV
#LCJetTightPPERCMissingEtAlg.jet_SoftJetsMaxPtCut        = 20.0*GeV
#LCJetTightPPERCMissingEtAlg.jet_ApplySoftJetsScale      = "No"
#LCJetTightPPERCMissingEtAlg.jet_calibType               ='LocHad'
#LCJetTightPPERCMissingEtAlg.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
#LCJetTightPPERCMissingEtAlg.ele_calibType               ='RefCalib'
#LCJetTightPPERCMissingEtAlg.gamma_calibType             ='EmScale'
#LCJetTightPPERCMissingEtAlg.cellout_calibType           ='Eflow'
#LCJetTightPPERCMissingEtAlg.cellout_PUSuppression_JA    =  False
#LCJetTightPPERCMissingEtAlg.tau_calibType               ='ExclRefCalib'
#LCJetTightPPERCMissingEtAlg.cryo_ApplyCorrection        = "Off"
#LCJetTightPPERCMissingEtAlg.muon_container              = "MuidMuonCollection"
#LCJetTightPPERCMissingEtAlg.muon_algorithm              = "Muid"
#LCJetTightPPERCMissingEtAlg.muon_isolationAlg           = "dRJet"
#LCJetTightPPERCMissingEtAlg()

my_alg1 = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_noEflow')
my_alg1.ele_calibType               ='ExclRefCalib'
my_alg1.gamma_calibType             ='EmScale'
my_alg1.tau_calibType               ='ExclRefCalib'
my_alg1.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
my_alg1.jet_JetPtCut                = 20.0*GeV
my_alg1.jet_calibType               ='ExclRefCalib'
my_alg1.jet_UseWeights              = True
my_alg1.jet_RunSoftJetsTool         = False
my_alg1.cellout_calibType           ='LocHad'
my_alg1.muon_algorithm              = "Staco"
my_alg1.muon_isolationAlg           = "dRJet"
my_alg1.cellout_PUSuppression_JA    = False
my_alg1()

my_alg2 = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_LCW_pt20')
my_alg2.ele_isEM                    = PyAthena.egammaPID.ElectronTight
my_alg2.ele_calibType               ='ExclRefCalib'
my_alg2.gamma_calibType             ='ExclRefCalib'
my_alg2.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
my_alg2.jet_JetPtCut                = 20.0*GeV
my_alg2.jet_ApplyJetScale           = "No"
my_alg2.jet_RunSoftJetsTool         = False
my_alg2.jet_calibType               ='LocHad'
my_alg2.cellout_calibType           ='LocHad'
my_alg2.cellout_PUSuppression_JA    = False
my_alg2.tau_doTauTool               = False
my_alg2.cryo_ApplyCorrection        = "Off"
my_alg2.muon_algorithm              = "Muid"
my_alg2.muon_isolationAlg           = "dRJet"
my_alg2()

#my_alg4 = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_LCW_NI_eflow6')
#my_alg4.ele_calibType               ='ExclRefCalib'
#my_alg4.gamma_calibType             ='EmScale'
#my_alg4.tau_calibType               ='ExclRefCalib'
#my_alg4.jet_JetInputCollectionKey   ='AntiKt6LCTopoNewJets'
#my_alg4.jet_JetPtCut                = 20.0*GeV
#my_alg4.jet_calibType               ='ExclRefCalib'
#my_alg4.jet_UseWeights              = True
#my_alg4.jet_RunSoftJetsTool         = False
#my_alg4.cellout_calibType           ='Eflow'
#my_alg4.muon_algorithm              = "Staco"
#my_alg4.muon_isolationAlg           = "dRJet"
#my_alg4.cellout_PUSuppression_JA    = False
#my_alg4()


my_alg6 = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_Eflow_PV')
my_alg6.ele_calibType               ='ExclRefCalib'
my_alg6.gamma_calibType             ='EmScale'
my_alg6.tau_calibType               ='ExclRefCalib'
my_alg6.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
my_alg6.jet_JetPtCut                = 20.0*GeV
my_alg6.jet_calibType               ='ExclRefCalib'
my_alg6.jet_UseWeights              = True
my_alg6.jet_RunSoftJetsTool         = False
my_alg6.cellout_calibType           ='Eflow'
my_alg6.cellout_EFlow_PV            = True
my_alg6.muon_algorithm              = "Staco"
my_alg6.muon_isolationAlg           = "dRJet"
my_alg6.cellout_PUSuppression_JA    = False
my_alg6()

my_alg7 = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_LCW_pt20_EM')
my_alg7.ele_isEM                    = PyAthena.egammaPID.ElectronTight
my_alg7.ele_calibType               ='ExclRefCalib'
my_alg7.gamma_calibType             ='ExclRefCalib'
my_alg7.jet_JetInputCollectionKey   ='AntiKt4TopoEMNewJets'
my_alg7.jet_JetPtCut                = 20.0*GeV
my_alg7.jet_ApplyJetScale           = "No"
my_alg7.jet_RunSoftJetsTool         = False
my_alg7.jet_calibType               ='EmScale'
my_alg7.cellout_calibType           ='EmScale'
my_alg7.cellout_PUSuppression_JA    = False
my_alg7.tau_doTauTool               = False
my_alg7.cryo_ApplyCorrection        = "Off"
my_alg7.muon_algorithm              = "Muid"
my_alg7.muon_isolationAlg           = "dRJet"
my_alg7()


# MEt for SUSY group
## defined in python/SUSYCustomMET.py
# end MEt for SUSY group

# MEt for SMWZ group
METRefAlg_STVFMuid = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_STVFMuid')
#METRefAlg_STVFMuid.sequence                    = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
METRefAlg_STVFMuid.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
METRefAlg_STVFMuid.jet_JetPtCut                = 20.0*GeV
METRefAlg_STVFMuid.jet_ApplyJetScale           = "Yes"
METRefAlg_STVFMuid.jet_UseJetMomentForScale    = True
METRefAlg_STVFMuid.jet_JetMomentForScale       = "JES"
METRefAlg_STVFMuid.jet_ApplyJetJVF             = "Yes"
METRefAlg_STVFMuid.jet_RunSoftJetsTool         = False
METRefAlg_STVFMuid.jet_calibType               ='LocHad'
METRefAlg_STVFMuid.ele_calibType               ='RefCalib'
METRefAlg_STVFMuid.gamma_calibType             ='EmScale'
METRefAlg_STVFMuid.plupSuppCorr                ='STVF'
METRefAlg_STVFMuid.celloutCorrection           ='STVF'
METRefAlg_STVFMuid.cellout_PUSuppression_JA    = False
METRefAlg_STVFMuid.cellout_calibType           ='Eflow'
METRefAlg_STVFMuid.tau_calibType               ='ExclRefCalib'
METRefAlg_STVFMuid.cryo_ApplyCorrection        = "Off"
METRefAlg_STVFMuid.muon_container              = "MuidMuonCollection"
METRefAlg_STVFMuid.muon_algorithm              = "Muid"
METRefAlg_STVFMuid.muon_isolationAlg           = "dRJet"
METRefAlg_STVFMuid()

METRefAlg_STVFMuons = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_STVFMuons')
#METRefAlg_STVFMuons.sequence                    = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
METRefAlg_STVFMuons.jet_JetInputCollectionKey   ='AntiKt4LCTopoNewJets'
METRefAlg_STVFMuons.jet_JetPtCut                = 20.0*GeV
METRefAlg_STVFMuons.jet_ApplyJetScale           = "Yes"
METRefAlg_STVFMuons.jet_UseJetMomentForScale    = True
METRefAlg_STVFMuons.jet_JetMomentForScale       = "JES"
METRefAlg_STVFMuons.jet_ApplyJetJVF             = "Yes"
METRefAlg_STVFMuons.jet_RunSoftJetsTool         = False
METRefAlg_STVFMuons.jet_calibType               ='LocHad'
METRefAlg_STVFMuons.ele_calibType               ='RefCalib'
METRefAlg_STVFMuons.gamma_calibType             ='EmScale'
METRefAlg_STVFMuons.plupSuppCorr                ='STVF'
METRefAlg_STVFMuons.celloutCorrection           ='STVF'
METRefAlg_STVFMuons.cellout_PUSuppression_JA    = False
METRefAlg_STVFMuons.cellout_calibType           ='Eflow'
METRefAlg_STVFMuons.tau_calibType               ='ExclRefCalib'
METRefAlg_STVFMuons.cryo_ApplyCorrection        = "Off"
METRefAlg_STVFMuons.muon_container              = "Muons"
METRefAlg_STVFMuons.muon_algorithm              = "Muons"
METRefAlg_STVFMuons.muon_isolationAlg           = "dRJet"
METRefAlg_STVFMuons()


### adding HSG5 MET
METRefAlg_HSG5 = MissingET.METRefGetter_plup.make_METRefAlg(_suffix='_HSG5')
METRefAlg_HSG5.sequence                    = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
METRefAlg_HSG5.jet_JetInputCollectionKey   = "AntiKt4TopoEMNewJets"
METRefAlg_HSG5.jet_JetPtCut                = 20.0*GeV
METRefAlg_HSG5.jet_ApplyJetScale           = "No"
METRefAlg_HSG5.jet_UseJetMomentForScale    = True
METRefAlg_HSG5.jet_JetMomentForScale       = "JES"
METRefAlg_HSG5.jet_RunSoftJetsTool         = False
METRefAlg_HSG5.jet_SoftJetsPtCut           = 7.0*GeV
METRefAlg_HSG5.jet_SoftJetsMaxPtCut        = 20.0*GeV
METRefAlg_HSG5.photon_doPhotonTool         = False
METRefAlg_HSG5.tau_doTauTool               = False
METRefAlg_HSG5.jet_SoftJetsCalibType       = "EmScale"
METRefAlg_HSG5.jet_ApplySoftJetsScale      = "No"
METRefAlg_HSG5.jet_calibType               = "ExclRefCalib"
METRefAlg_HSG5.ele_calibType               = "RefCalib"
METRefAlg_HSG5.gamma_calibType             = "EmScale"
METRefAlg_HSG5.cellout_calibType           = "Eflow"
METRefAlg_HSG5.cellout_PUSuppression_JA    = False
METRefAlg_HSG5.tau_calibType               = "EmScale"
METRefAlg_HSG5.cryo_ApplyCorrection        = "Off"
METRefAlg_HSG5.muon_container              = "MuidMuonCollection"
METRefAlg_HSG5.muon_algorithm              = "Muid"
METRefAlg_HSG5.muon_isolationAlg           = "dRJet"
print METRefAlg_HSG5
METRefAlg_HSG5()


# end MEt for SMWZ group

# MEt for TAU group

# end MEt for TAU group                                                     
