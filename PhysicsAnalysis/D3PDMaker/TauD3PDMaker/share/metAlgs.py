from MissingET.METRefGetter_new import *
from MissingET.METRefGetter_plup import *
#Tau/MET new Baseline
my_alg1 = make_METRefAlg(_suffix='_BDTMedium')
my_alg1.tau_doTauTool               = True
my_alg1.tau_isTau                   ='BDTMedium_EleBDTMedium_MuonVeto'
my_alg1.tau_calibType               ='ExclRefCalib'
my_alg1.tau_TauSignalConeSize       = 0.3
my_alg1.tau_TauPtCut                = 20.*GeV
my_alg1()


# METRefFinal with tight++ electrons, tau term is switched off, muid used (request by the H+ group)
my_alg2 = make_METRefAlg(_suffix = "_AntiKt4LCTopo_tightpp")
my_alg2.ele_EleInputCollectionKey   = "ElectronAODCollection"
my_alg2.ele_noCut                   = False
my_alg2.ele_isEM                    = PyAthena.egammaPID.ElectronIDTightPP
my_alg2.ele_calibType               = "RefCalib"
my_alg2.photon_doPhotonTool         = False
my_alg2.gamma_isEM                  = PyAthena.egammaPID.PhotonIDTight
my_alg2.gamma_calibType             = "EmScale"
my_alg2.tau_doTauTool               = False
my_alg2.tau_isTau                   ="Tight"
my_alg2.tau_calibType               = "EmScale"
my_alg2.tau_TauPtCut                = -1.*GeV
my_alg2.tau_TauSignalConeSize       = -1.
my_alg2.jet_JetInputCollectionKey   = "AntiKt4LCTopoJets"
my_alg2.jet_calibType               = "ExclRefCalib"
my_alg2.jet_ApplySoftJetsScale      = "No"
my_alg2.cellout_JetInputCollectionKey = "AntiKt4LCTopoJets"
my_alg2.cellout_calibType           = "Eflow"
my_alg2.cryo_ApplyCorrection        = "Off"
my_alg2.muon_container              = "MuidMuonCollection"
my_alg2.muon_algorithm              = "Muid"
my_alg2.muon_isolationAlg           = "dRJet"
my_alg2()

 


