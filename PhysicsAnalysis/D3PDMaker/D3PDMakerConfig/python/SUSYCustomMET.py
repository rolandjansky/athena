# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MissingETD3PDMaker.MissingETD3PDMakerFlags      import MissingETD3PDMakerFlags
from MissingETD3PDMaker.MissingETD3PDObject          import MissingETD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject          import MissingETCompositionD3PDObject
from MissingETD3PDMaker.MissingETGoodnessD3PDObject  import MissingETGoodnessD3PDObject
from SUSYD3PDMaker.SUSYD3PDFlags                     import SUSYD3PDFlags
#from MissingET.METRefGetter_new                      import *
from MissingET.METRefGetter_plup                  import *
from RecExConfig.RecFlags                            import rec

##############################################################################################################
##############################################################################################################
##############################################################################################################

def addSUSYCustomMissingET( topSequence ):

    #--------------------------------------
    # recalculate MET_RefFinal for studies
    #--------------------------------------

    #-- MET_RefFinal with no taus : PU suppressed MET also calculated.
    newRefFinal_Egamma10NoTau_alg =     make_METRefAlg(_suffix='_Egamma10NoTau')
    newRefFinal_Egamma10NoTau_alg.tau_doTauTool               = False
    newRefFinal_Egamma10NoTau_alg.cellout_calibType           = "Eflow"
    newRefFinal_Egamma10NoTau_alg.jet_calibType               = 'ExclRefCalib'
    newRefFinal_Egamma10NoTau_alg.jet_JetInputCollectionKey   = "AntiKt4LCTopoNewJets"
    newRefFinal_Egamma10NoTau_alg.cellout_PUSuppression_JA    = True
    newRefFinal_Egamma10NoTau_alg()

    # below not need anymore

    #if(SUSYD3PDFlags.FixMCTruthBug()): jetSGKey_TopoEM = 'AntiKt4TopoEMJetsAOD' 
    #else: jetSGKey_TopoEM = 'AntiKt4TopoEMJets'
   
    #if(SUSYD3PDFlags.do01LepFactoryNtuple() or SUSYD3PDFlags.do23Lep1TauFactoryNtuple() or SUSYD3PDFlags.doRazorFactoryNtuple()): 
    #    simp20_alg = make_METRefAlg(_suffix='_Simplified20')
    #    simp20_alg.jet_JetInputCollectionKey   = jetSGKey_TopoEM
    #    simp20_alg.jet_JetPtCut                = 20.0*GeV
    #    simp20_alg.ele_EtCut                   = 20.0*GeV
    #    simp20_alg.jet_ApplyJetScale           = "Yes"
    #    simp20_alg.jet_UseJetMomentForScale    = True
    #    simp20_alg.jet_JetMomentForScale       = "EMJES"
    #    simp20_alg.jet_RunSoftJetsTool         = False
    #    simp20_alg.jet_SoftJetsPtCut           = 7.0*GeV
    #    simp20_alg.jet_SoftJetsMaxPtCut        = 20.0*GeV
    #    simp20_alg.photon_doPhotonTool         = False
    #    simp20_alg.tau_doTauTool               = False
    #    simp20_alg.jet_SoftJetsCalibType       = "EmScale"
    #    simp20_alg.jet_ApplySoftJetsScale      = "No"
    #    simp20_alg.jet_calibType               ='EmScale'
    #    simp20_alg.ele_calibType               ='RefCalib'
    #    simp20_alg.gamma_calibType             ='EmScale'
    #    simp20_alg.cellout_calibType           ='EmScale'
    #    simp20_alg.tau_calibType               ='EmScale'
    #    simp20_alg.cryo_ApplyCorrection        = "Off"
    #    simp20_alg.muon_container              = "StacoMuonCollection"
    #    simp20_alg.muon_algorithm              = "Staco"
    #    simp20_alg.muon_isolationAlg           = "dRJet"
    #    simp20_alg()
    
        # exactly the same as Simplified20, only the ele_EtCut is 10 GeV (below) vs 20 GeV (Simplified20)
    #    simp20Elec10_alg = make_METRefAlg(_suffix='_Simplified20Elec10')
    #    simp20Elec10_alg.jet_JetInputCollectionKey   = jetSGKey_TopoEM
    #    simp20Elec10_alg.jet_JetPtCut                = 20.0*GeV 
    #    simp20Elec10_alg.ele_EtCut                   = 10.0*GeV # this is default, but writing here to be explicit
    #    simp20Elec10_alg.jet_ApplyJetScale           = "Yes"
    #    simp20Elec10_alg.jet_UseJetMomentForScale    = True
    #    simp20Elec10_alg.jet_JetMomentForScale       = "EMJES"
    #    simp20Elec10_alg.jet_RunSoftJetsTool         = False
    #    simp20Elec10_alg.jet_SoftJetsPtCut           = 7.0*GeV
    #    simp20Elec10_alg.jet_SoftJetsMaxPtCut        = 20.0*GeV
    #    simp20Elec10_alg.photon_doPhotonTool         = False
    #    simp20Elec10_alg.tau_doTauTool               = False
    #    simp20Elec10_alg.jet_SoftJetsCalibType       = "EmScale"
    #    simp20Elec10_alg.jet_ApplySoftJetsScale      = "No"
    #    simp20Elec10_alg.jet_calibType               ='EmScale'
    #    simp20Elec10_alg.ele_calibType               ='RefCalib'
    #    simp20Elec10_alg.gamma_calibType             ='EmScale'
    #    simp20Elec10_alg.cellout_calibType           ='EmScale'
    #    simp20Elec10_alg.tau_calibType               ='EmScale'
    #    simp20Elec10_alg.cryo_ApplyCorrection        = "Off"
    #    simp20Elec10_alg.muon_container              = "StacoMuonCollection"
    #    simp20Elec10_alg.muon_algorithm              = "Staco"
    #    simp20Elec10_alg.muon_isolationAlg           = "dRJet"
    #    simp20Elec10_alg()

        # exactly the same as Simplified20Elec10, only including Tau's 
    #    simp20Elec10WithTaus_alg = make_METRefAlg(_suffix='_Simplified20Elec10WithTaus')
    #    simp20Elec10WithTaus_alg.jet_JetInputCollectionKey   = jetSGKey_TopoEM
    #    simp20Elec10WithTaus_alg.jet_JetPtCut                = 20.0*GeV 
    #    simp20Elec10WithTaus_alg.ele_EtCut                   = 10.0*GeV # this is default, but writing here to be explicit
    #    simp20Elec10WithTaus_alg.jet_ApplyJetScale           = "Yes"
    #    simp20Elec10WithTaus_alg.jet_UseJetMomentForScale    = True
    #    simp20Elec10WithTaus_alg.jet_JetMomentForScale       = "EMJES"
    #    simp20Elec10WithTaus_alg.jet_RunSoftJetsTool         = False
    #    simp20Elec10WithTaus_alg.jet_SoftJetsPtCut           = 7.0*GeV
    #    simp20Elec10WithTaus_alg.jet_SoftJetsMaxPtCut        = 20.0*GeV
    #    simp20Elec10WithTaus_alg.photon_doPhotonTool         = False
    #    simp20Elec10WithTaus_alg.tau_doTauTool               = True  
    #    simp20Elec10WithTaus_alg.jet_SoftJetsCalibType       = "EmScale"
    #    simp20Elec10WithTaus_alg.jet_ApplySoftJetsScale      = "No"
    #    simp20Elec10WithTaus_alg.jet_calibType               ='EmScale'
    #    simp20Elec10WithTaus_alg.ele_calibType               ='RefCalib'
    #    simp20Elec10WithTaus_alg.gamma_calibType             ='EmScale'
    #    simp20Elec10WithTaus_alg.cellout_calibType           ='EmScale'
    #    simp20Elec10WithTaus_alg.cryo_ApplyCorrection        = "Off"
    #    simp20Elec10WithTaus_alg.muon_container              = "StacoMuonCollection"
    #    simp20Elec10WithTaus_alg.muon_algorithm              = "Staco"
    #    simp20Elec10WithTaus_alg.muon_isolationAlg           = "dRJet"
    #    simp20Elec10WithTaus_alg()

    #if SUSYD3PDFlags.doLArHoleFix() and False:
    #    metLArFix_alg = make_METRefAlg(_suffix='_LArFixSimp20')
    #    metLArFix_alg.jet_JetInputCollectionKey   = "AntiKtTopoEMJetsLArFix"
    #    metLArFix_alg.jet_JetPtCut                = 20.0*GeV
    #    metLArFix_alg.ele_EtCut                   = 20.0*GeV
    #    metLArFix_alg.jet_ApplyJetScale           = "Yes"
    #    metLArFix_alg.jet_UseJetMomentForScale    = True
    #    metLArFix_alg.jet_JetMomentForScale       = "EMJES"
    #    metLArFix_alg.jet_RunSoftJetsTool         = False
    #    metLArFix_alg.jet_SoftJetsPtCut           = 7.0*GeV
    #    metLArFix_alg.jet_SoftJetsMaxPtCut        = 20.0*GeV
    #    metLArFix_alg.photon_doPhotonTool         = False
    #    metLArFix_alg.tau_doTauTool               = False
    #    metLArFix_alg.jet_SoftJetsCalibType       = "EmScale"
    #    metLArFix_alg.jet_ApplySoftJetsScale      = "No"
    #    metLArFix_alg.jet_calibType               ='EmScale'
    #    metLArFix_alg.ele_calibType               ='RefCalib'
    #    metLArFix_alg.gamma_calibType             ='EmScale'
    #    metLArFix_alg.cellout_calibType           ='EmScale'
    #    metLArFix_alg.tau_calibType               ='EmScale'
    #    metLArFix_alg.cryo_ApplyCorrection        = "Off"
    #    metLArFix_alg.muon_container              = "StacoMuonCollection"
    #    metLArFix_alg.muon_algorithm              = "Staco"
    #    metLArFix_alg.muon_isolationAlg           = "dRJet"
    #    metLArFix_alg()
    #    topSequence.METRefAlg_LArFixSimp20.METRefCluster_LArFixSimp20.ClusterInputCollectionKey = 'SUSYClusterCandidate'

    return 

