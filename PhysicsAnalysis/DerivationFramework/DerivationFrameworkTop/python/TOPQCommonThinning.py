# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used for TOPQ thinning
# Call with:
#   import DerivationFrameworkTop.TOPQCommonThinning
#   thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQX',ThinningSvc, ToolSvc)
#   TOPQXThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains()
# Tools used:
#   * TriggerNavigationThinning
#   * TrackParticleThinning ( !!! NOT CURRENTLY BEING USED !!! )
#   * JetTrackParticleThinning
#   * ElectronTrackParticleThinning
#   * ElectronCaloClusterThinning
#   * PhotonCaloClusterThinning
#   * MuonTrackParticleThinning
#   * TauTrackParticleThinning
#   * MenuTruthThinning (course-grained thinning)
#   * GenericTruthThinning (fine-grained thinning)
#====================================================================

#============================
# Define trigger chain output
#============================
def TOPQTriggerChains():
  electronTriggers='HLT_e.*lhloose.*|HLT_e.*lhmedium.*|HLT_e.*lhtight.*|HLT_e.*lhvloose.*|HLT_2e.*'
  muonTriggers='HLT_mu.*|HLT_2mu.*'
  elecPlusMuTriggers='HLT_e.*mu.*|HLT_mu.*e.*|'
  tauTriggers='HLT_tau.*'
  jetTriggers='HLT_3j.*|HLT_4j.*|HLT_5j.*|HLT_6j.*|HLT_7j.*|HLT_8j.*|HLT_j.*_a.*| HLT_ht.*'
  bjetTriggers='HLT_.*bmedium.*|HLT_.*btight.*|HLT_.*bloose.*'
  TriggerChains=electronTriggers+"|"+muonTriggers+"|"+tauTriggers+"|"+jetTriggers+"|"+bjetTriggers+"|"+elecPlusMuTriggers
  print "TriggerChains: ", TriggerChains
  return TriggerChains
# end TOPQTriggerChains()

#============================
# MAIN SETUP FUNCTION
#============================
def setup(TOPQname, TOPQThinningSvc, ToolSvc):
  thinningTools=[]

  #========================
  # Track Particle Thinning !!! BUGGY (yes, that's a technical term)...CURRENTLY NOT USING !!!
  #========================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/TrackParticleThinning.cxx
  thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
  TOPQTPThinningTool = DerivationFramework__TrackParticleThinning(  
                         name                    = TOPQname + "TPThinningTool",
						 ThinningService         = TOPQThinningSvc,
						 SelectionString         = thinning_expression,
						 InDetTrackParticlesKey  = "InDetTrackParticles",
						 ApplyAnd                = False)

  #ToolSvc += TOPQTPThinningTool
  #thinningTools.append(TOPQTPThinningTool)
  #print TOPQname+".py", TOPQname+"TPThinningTool: ", TOPQTPThinningTool
  
  #============================
  # Jet Track Particle Thinning
  #============================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/JetTrackParticleThinning.cxx
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
  TOPQJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
                            name                    = TOPQname + "JetTPThinningTool",
                            ThinningService         = TOPQThinningSvc,
                            JetKey                  = "AntiKt4EMTopoJets",
                            InDetTrackParticlesKey  = "InDetTrackParticles",
                            ApplyAnd                = False)
                            
  ToolSvc += TOPQJetTPThinningTool
  thinningTools.append(TOPQJetTPThinningTool)
  print TOPQname+".py", TOPQname+"JetTPThinningTool: ", TOPQJetTPThinningTool

  #=================================
  # Electron Track Particle Thinning
  #=================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/EgammaTrackParticleThinning.cxx
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
  TOPQElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
                                 name                    = TOPQname + "ElectronTPThinningTool",
                                 ThinningService         = TOPQThinningSvc,
                                 SGKey                   = "Electrons",
                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                 ApplyAnd                = False)

  ToolSvc += TOPQElectronTPThinningTool
  thinningTools.append(TOPQElectronTPThinningTool)
  print TOPQname+".py", TOPQname+"ElectronTPThinningTool: ", TOPQElectronTPThinningTool

  #===============================
  # Electron Calo Cluster Thinning
  #===============================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkCalo/trunk/src/CaloClusterThinning.cxx
  from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
  TOPQElectronCCThinningTool = DerivationFramework__CaloClusterThinning(
                                 name                    = TOPQname + "ElectronCCThinningTool",
                                 ThinningService         = TOPQThinningSvc,
                                 SGKey                   = "Electrons",
                                 CaloClCollectionSGKey   = "egammaClusters",
                                 SelectionString         = "(Electrons.Loose || Electrons.DFCommonElectronsLHLoose)",
                                 ConeSize                = 0.4,
                                 ApplyAnd                = False)

  ToolSvc += TOPQElectronCCThinningTool
  thinningTools.append(TOPQElectronCCThinningTool)
  print TOPQname+".py", TOPQname+"ElectronCCThinningTool: ", TOPQElectronCCThinningTool
  
  #===============================
  # Photon Calo Cluster Thinning
  #===============================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkCalo/trunk/src/CaloClusterThinning.cxx
  from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
  TOPQPhotonCCThinningTool = DerivationFramework__CaloClusterThinning(
                                 name                    = TOPQname + "PhotonCCThinningTool",
                                 ThinningService         = TOPQThinningSvc,
                                 SGKey                   = "Photons",
                                 CaloClCollectionSGKey   = "egammaClusters",
                                 SelectionString         = "(Photons.Loose || Photons.DFCommonPhotonsIsEMLoose)",
                                 ConeSize                = 0.4,
                                 ApplyAnd                = False)

  ToolSvc += TOPQPhotonCCThinningTool
  thinningTools.append(TOPQPhotonCCThinningTool)
  print TOPQname+".py", TOPQname+"PhotonCCThinningTool: ", TOPQPhotonCCThinningTool
  
  #=============================
  # Muon Track Particle Thinning
  #=============================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/MuonTrackParticleThinning.cxx
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
  TOPQMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
                             name                    = TOPQname + "MuonTPThinningTool",
                             ThinningService         = TOPQThinningSvc,
                             MuonKey                 = "Muons",
                             InDetTrackParticlesKey  = "InDetTrackParticles",
                             ApplyAnd                = False)

  ToolSvc += TOPQMuonTPThinningTool
  thinningTools.append(TOPQMuonTPThinningTool)
  print TOPQname+".py", TOPQname+"MuonTPThinningTool: ", TOPQMuonTPThinningTool

  #============================
  # Tau Track Particle Thinning
  #============================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/TauTrackParticleThinning.cxx
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
  TOPQTauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
                            name                    = TOPQname + "TauTPThinningTool",
                            ThinningService         = TOPQThinningSvc,
                            TauKey                  = "TauJets",
                            ConeSize                = 0.6,
                            InDetTrackParticlesKey  = "InDetTrackParticles",
                            ApplyAnd                = False)

  ToolSvc += TOPQTauTPThinningTool
  thinningTools.append(TOPQTauTPThinningTool)
  print TOPQname+".py", TOPQname+"TauTPThinningTool: ", TOPQTauTPThinningTool
  
  #==============================================
  # TRUTH THINNING
  #==============================================
  from AthenaCommon.GlobalFlags import globalflags
  if globalflags.DataSource()=='geant4':
    #==========================
    # MENU TRUTH THINNING
    #==========================
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    TOPQTruthThinningTool = DerivationFramework__MenuTruthThinning(
                              name                       = TOPQname + "TruthThinningTool",
                              ThinningService            = TOPQThinningSvc,
                              ParticlesKey               = "TruthParticles",
                              VerticesKey                = "TruthVertices",
                              EventsKey                  = "TruthEvents",
                              WritePartons               = True,   # keep partons?  ###maybe set to FALSE?
                              WriteHadrons               = False,  # keep hadrons?
                              WriteBHadrons	             = False,  # keep b-hadrons?
                              WriteGeant                 = False,  # keep Geant particles?
                              GeantPhotonPtThresh        = -1.0,   # Set to <0 to not write any Geant photons; otherwise write with a pT above threshold
                              WriteTauHad                = True,   # keep hadronic taus?
                              PartonPtThresh             = -1.0,   # write partons with pT aboe this threhold
                              WriteBSM 	                 = True,   # keep BSM particles?
                              WriteBSMProducts           = True,   # keep BSM particle decay products?
                              WriteBosons 	             = True,   # keep bosons?
                              WriteBosonProducts         = True,   # keep boson decay products?
                              WriteTopAndDecays          = True,   # keep top partons and immediate decay products?
                              WriteEverything            = False,  # keep everything?; overrides all other flags
                              WriteAllLeptons            = True,   # keep absolutely all leptons?
                              WriteLeptonsNotFromHadrons = False,  # keep leptons not from hadron decays
                              WriteAllStable             = True,   # keep all stable particles?   ###maybe set to FALSE?
                              WriteStatus3 	             = False,  # keep all particles with status code 3?
                              WriteFirstN                = 10,     # keep first N particles in record
                              PreserveDescendants        = False,  # keep descendants of retained particles?
                              PreserveAncestors          = True,   # keep ancestors of retained particles?
                              SimBarcodeOffset           = 200000, # barcode offset for simulation particles
                              WritettHFHadrons           = True)   # keep tt+HF hadrons         
                              
    ToolSvc += TOPQTruthThinningTool
    thinningTools.append(TOPQTruthThinningTool)
    print TOPQname+".py", TOPQname+"TruthThinningTool: ", TOPQTruthThinningTool
    
    #==============================================================================
    # Thinning the photon truth collection : no photons from pi0 (origin=42)
    #==============================================================================
    # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/GenericTruthThinning.cxx
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    TOPQPhotonThinning = DerivationFramework__GenericTruthThinning(
                           name                    = TOPQname + "PhotonThinning",
                           ThinningService         = TOPQThinningSvc,
                           ParticlesKey            = "TruthPhotons", 
                           ParticleSelectionString = "(TruthPhotons.classifierParticleOrigin != 42) || (TruthPhotons.pt > 20.0*GeV)")

    ToolSvc += TOPQPhotonThinning
    thinningTools.append(TOPQPhotonThinning)
    print TOPQname+".py", TOPQname+"PhotonThinning: ", TOPQPhotonThinning
    
    #==============================================================================
    # Thinning TruthParticles : no gluons
    #==============================================================================
    ## PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/GenericTruthThinning.cxx
    #from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    #TOPQGluonThinning = DerivationFramework__GenericTruthThinning(
    #                      name                    = TOPQname + "GluonThinning",
    #                      ThinningService         = TOPQThinningSvc,
    #                      ParticlesKey            = "TruthParticles", 
    #                      ParticleSelectionString = "(TruthParticles.pdgId  != 21)")

    #ToolSvc += TOPQGluonThinning
    #thinningTools.append(TOPQGluonThinning)
    #print TOPQname+".py", TOPQname+"GluonThinning: ", TOPQGluonThinning
  
  return thinningTools
# end setup(TOPQname, TOPQThinningSvc, ToolSvc)
