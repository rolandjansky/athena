# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
def TOPQTriggerChains(TriggerFilter='allTriggers'):
    electronTriggers   = '^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhloose.*|^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhmedium.*|^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhtight.*|^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhvloose.*|HLT_2e.*'
    muonTriggers       = '^(?!.*_[0-9]*(e|j|xe|tau|ht|xs|te))(?!HLT_mu.*_[0-9]*mu.*)HLT_mu.*|HLT_2mu.*'
    elecPlusMuTriggers = 'HLT_e.*mu.*|HLT_mu.*e.*'
    tauTriggers        = 'HLT_tau.*'
    jetTriggers        = 'HLT_3j.*|HLT_4j.*|HLT_5j.*|HLT_6j.*|HLT_7j.*|HLT_8j.*|HLT_j.*_a.*| HLT_ht.*'
    bjetTriggers       = 'HLT_.*bmedium.*|HLT_.*btight.*|HLT_.*bloose.*|HLT_.*boffperf.*|HLT_.*bmv2.*'
    TriggerChains      = ''
    if TriggerFilter=='allTriggers':
        TriggerChains      = electronTriggers+"|"+muonTriggers+"|"+tauTriggers+"|"+jetTriggers+"|"+bjetTriggers+"|"+elecPlusMuTriggers
    elif TriggerFilter=='leptonicTriggers':
        TriggerChains      = electronTriggers+"|"+muonTriggers+"|"+tauTriggers+"|"+elecPlusMuTriggers
    elif TriggerFilter=='hadronicTriggers':
        TriggerChains      = jetTriggers+"|"+bjetTriggers
    else:
        print 'Unknown TriggerFilter parameter \"'+TriggerFilter+'\" - acting as \"allTriggers\"'
    print "TOPQ triggers kept: ", TriggerChains
    return TriggerChains
# end TOPQTriggerChains()

#============================
# MAIN SETUP FUNCTION
#============================
def setup(TOPQname, TOPQThinningSvc, ToolSvc):
    from AthenaCommon.GlobalFlags import globalflags

    thinningTools=[]

    DFisMC = (globalflags.DataSource()=='geant4')

    #========================
    # Track Particle Thinning
    #========================

    # All tracks (for TOPQ6)
    if TOPQname == 'TOPQ6':
        thinning_expression = "(InDetTrackParticles.pt > 0.1*GeV)"
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
        TOPQTPThinningTool = DerivationFramework__TrackParticleThinning(
            name                    = TOPQname + "TPThinningTool",
            ThinningService         = TOPQThinningSvc,
            SelectionString         = thinning_expression,
            InDetTrackParticlesKey  = "InDetTrackParticles",
            ApplyAnd                = False
        )
        ToolSvc += TOPQTPThinningTool
        thinningTools.append(TOPQTPThinningTool)
        print TOPQname+".py", TOPQname+"TPThinningTool: ", TOPQTPThinningTool

    # Tracks associated to Muons
    # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/MuonTrackParticleThinning.cxx
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
    TOPQMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
        name                    = TOPQname + "MuonTPThinningTool",
        ThinningService         = TOPQThinningSvc,
        MuonKey                 = "Muons",
        InDetTrackParticlesKey  = "InDetTrackParticles",
        ApplyAnd                = False
    )
    ToolSvc += TOPQMuonTPThinningTool
    thinningTools.append(TOPQMuonTPThinningTool)
    print TOPQname+".py", TOPQname+"MuonTPThinningTool: ", TOPQMuonTPThinningTool

    # Tracks associated with Electrons
    # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/EgammaTrackParticleThinning.cxx
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
    TOPQElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
        name                    = TOPQname + "ElectronTPThinningTool",
        ThinningService         = TOPQThinningSvc,
        SGKey                   = "Electrons",
        BestMatchOnly           = False,
        InDetTrackParticlesKey  = "InDetTrackParticles",
        ApplyAnd                = False
    )
    ToolSvc += TOPQElectronTPThinningTool
    thinningTools.append(TOPQElectronTPThinningTool)
    print TOPQname+".py", TOPQname+"ElectronTPThinningTool: ", TOPQElectronTPThinningTool

    # Tracks associated to Photons
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
    TOPQPhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
        name                    = TOPQname + "PhotonTPThinningTool",
        ThinningService         = TOPQThinningSvc,
        SGKey                   = "Photons",
        InDetTrackParticlesKey  = "InDetTrackParticles"
    )
    ToolSvc += TOPQPhotonTPThinningTool
    thinningTools.append(TOPQPhotonTPThinningTool)
    print TOPQname+".py", TOPQname+"PhotonTPThinningTool: ", TOPQPhotonTPThinningTool

    # Tau Track Particle Thinning
    # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/TauTrackParticleThinning.cxx
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
    TOPQTauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
        name                    = TOPQname + "TauTPThinningTool",
        ThinningService         = TOPQThinningSvc,
        TauKey                  = "TauJets",
        InDetTrackParticlesKey  = "InDetTrackParticles",
        ApplyAnd                = False
    )
    ToolSvc += TOPQTauTPThinningTool
    thinningTools.append(TOPQTauTPThinningTool)
    print TOPQname+".py", TOPQname+"TauTPThinningTool: ", TOPQTauTPThinningTool

    # Tracks associated to AntiKt4EMTopoJets
    # On MC we need to use a lower calibrated jet pT cut because a systematic
    # variation might just fluctuate above threshold.
    if DFisMC:
        track_particle_thinning_jetpt_cut =  "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV)"
    else:
        track_particle_thinning_jetpt_cut =  "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20*GeV)"

    # PhysicsAnalysis/DerivationFramework/DerivationFrameworkInDet/trunk/src/JetTrackParticleThinning.cxx
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
    TOPQJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
        name                    = TOPQname + "JetTPThinningTool",
        ThinningService         = TOPQThinningSvc,
        JetKey                  = "AntiKt4EMTopoJets",
        InDetTrackParticlesKey  = "InDetTrackParticles",
        SelectionString         = track_particle_thinning_jetpt_cut,
        ApplyAnd                = False
    )
    ToolSvc += TOPQJetTPThinningTool
    thinningTools.append(TOPQJetTPThinningTool)
    print TOPQname+".py", TOPQname+"JetTPThinningTool: ", TOPQJetTPThinningTool

    # Tracks associated to Large Jet AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
    if not TOPQname == 'TOPQ6':
        TOPQLargeJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
            name                    = TOPQname + "LargeJetTPThinningTool",
            ThinningService         = TOPQThinningSvc,
            JetKey                  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
            InDetTrackParticlesKey  = "InDetTrackParticles",
            SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>25*GeV",
            ApplyAnd                = False
        )
        ToolSvc += TOPQLargeJetTPThinningTool
        thinningTools.append(TOPQLargeJetTPThinningTool)
        print TOPQname+".py", TOPQname+"LargeJetTPThinningTool: ", TOPQLargeJetTPThinningTool

    # Tracks associated to AntiKt4EMTopoJets btagged SV1
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__SV1TrackThinning
    TOPQSV1ThinningTool = DerivationFramework__SV1TrackThinning(
        name                    = TOPQname + "SV1TrackThinning",
        ThinningService         = TOPQThinningSvc,
        JetKey                  = "AntiKt4EMTopoJets",
        InDetTrackParticlesKey  = "InDetTrackParticles",
        SelectionString         = track_particle_thinning_jetpt_cut,
        ApplyAnd                = False
    )
    ToolSvc += TOPQSV1ThinningTool
    thinningTools.append(TOPQSV1ThinningTool)
    print TOPQname+".py", TOPQname+"SV1ThinningTool: ", TOPQSV1ThinningTool

    if TOPQname == 'TOPQ1' or TOPQname == 'TOPQ4':
        # Keep tracks associated to AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903
        # as well as their JetFitter and SV1 vertices
        # we are only interested in the latest calibrated time-stamped version
        # the largeRsel is to only keep tracks and vertices if events have boosted tops
        largeR_pt = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV"
        largeR_eta = "abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.5"
        largeRsel = "count(({0}) && ({1})) >= 1".format(largeR_pt, largeR_eta)
        track_particle_thinning_trackjetpt_cut = "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903.pt >= 10*GeV"

        from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TrkJetTrackThinning
        TOPQTrkJetThinningTool = DerivationFramework__TrkJetTrackThinning(
            name                    = TOPQname + "TrkJetTrackThinning",
            ThinningService         = TOPQThinningSvc,
            TrkJetKey               = "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
            InDetTrackParticlesKey  = "InDetTrackParticles",
            TrkJetSelectionString   = track_particle_thinning_trackjetpt_cut,
            EventSelectionString    = largeRsel,
            ThinConstituents        = True,
            ThinJetFitterTracks     = True,
            ThinSV1Tracks           = True,
            ApplyAnd                = False
        )
        ToolSvc += TOPQTrkJetThinningTool
        thinningTools.append(TOPQTrkJetThinningTool)
        print TOPQname+".py", TOPQname+"TrkJetThinningTool: ", TOPQTrkJetThinningTool

        from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TrkJetSecVtxThinning
        TOPQTrkJetSecVtxThinningTool = DerivationFramework__TrkJetSecVtxThinning(
            name                     = TOPQname + "TrkJetSecVtxThinning",
            ThinningService          = TOPQThinningSvc,
            TrkJetKey                = "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
            SecVtxContainerKey       = "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtx",
            TrkJetSelectionString    = track_particle_thinning_trackjetpt_cut,
            EventSelectionString     = largeRsel,
            ApplyAnd                 = False
        )
        ToolSvc += TOPQTrkJetSecVtxThinningTool
        thinningTools.append(TOPQTrkJetSecVtxThinningTool)
        print TOPQname+".py", TOPQname+"TrkJetSecVtxThinningTool: ", TOPQTrkJetSecVtxThinningTool

        from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TrkJetJetFitterThinning
        TOPQTrkJetJetFitterThinningTool = DerivationFramework__TrkJetJetFitterThinning(
            name                     = TOPQname + "TrkJetJetFitterThinning",
            ThinningService          = TOPQThinningSvc,
            TrkJetKey                = "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
            JetFitterContainerKey    = "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtx",
            TrkJetSelectionString    = track_particle_thinning_trackjetpt_cut,
            EventSelectionString     = largeRsel,
            ApplyAnd                 = False
        )
        ToolSvc += TOPQTrkJetJetFitterThinningTool
        thinningTools.append(TOPQTrkJetJetFitterThinningTool)
        print TOPQname+".py", TOPQname+"TrkJetJetFitterThinningTool: ", TOPQTrkJetJetFitterThinningTool

    if TOPQname == 'TOPQ1':

        # Keep tracks of the exkt2 jets

        largeR_pt = "AntiKt8EMPFlowJets.pt > 15*GeV"
        largeR_eta = "abs(AntiKt8EMPFlowJets.eta) < 2.5"
        largeRsel = "count(({0}) && ({1})) >= 1".format(largeR_pt, largeR_eta)
        track_particle_thinning_trackjetpt_cut = "AntiKt8EMPFlowExKt2GASubJets.pt >= 5*GeV"
        
        TOPQTrkJetThinningToolExkt = DerivationFramework__TrkJetTrackThinning(
            name                    = TOPQname + "TrkJetTrackThinning",
            ThinningService         = TOPQThinningSvc,
            TrkJetKey               = "AntiKt8EMPFlowExKt2GASubJets",
            InDetTrackParticlesKey  = "InDetTrackParticles",
            TrkJetSelectionString   = track_particle_thinning_trackjetpt_cut,
            EventSelectionString    = largeRsel,
            ThinConstituents        = True,
            ThinJetFitterTracks     = False,
            ThinSV1Tracks           = False,
            ApplyAnd                = False
        )
        ToolSvc += TOPQTrkJetThinningToolExkt
        thinningTools.append(TOPQTrkJetThinningToolExkt)
        print TOPQname+".py", TOPQname+"TrkJetThinningTool: ", TOPQTrkJetThinningToolExkt

    #============================
    # JetCaloCluster Thinning (AntiKt4EMTopoJets)
    #============================
    from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
    TOPQAK4CCThinningTool = DerivationFramework__JetCaloClusterThinning(
        name                    = TOPQname + "AK4CCThinningTool",
        ThinningService         = TOPQThinningSvc,
        SGKey                   = "AntiKt4EMTopoJets",
        TopoClCollectionSGKey   = "CaloCalTopoClusters",
        SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 7*GeV",
        AdditionalClustersKey = ["EMOriginTopoClusters","LCOriginTopoClusters","CaloCalTopoClusters"]
    )
    ToolSvc += TOPQAK4CCThinningTool
    thinningTools.append(TOPQAK4CCThinningTool)
    print TOPQname+".py", TOPQname+"AK4CCThinningTool: ", TOPQAK4CCThinningTool


    #============================
    # JetCaloCluster Thinning (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets)
    #============================
    # Apply this to all derivations but TOPQ6 as it does not contain large-R jets.
    if not TOPQname == 'TOPQ6':
        from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
        TOPQLargeJetCCThinningTool = DerivationFramework__JetCaloClusterThinning(
            name                    = TOPQname + "LargeJetCCThinningTool",
            ThinningService         = TOPQThinningSvc,
            SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
            TopoClCollectionSGKey   = "CaloCalTopoClusters",
            SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 7*GeV",
            AdditionalClustersKey = ["EMOriginTopoClusters","LCOriginTopoClusters","CaloCalTopoClusters"]
        )
        ToolSvc += TOPQLargeJetCCThinningTool
        thinningTools.append(TOPQLargeJetCCThinningTool)
        print TOPQname+".py", TOPQname+"LargeJetCCThinningTool: ", TOPQLargeJetCCThinningTool


    #===============================
    # Calo Cluster Thinning (associated to electrons)
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
        ApplyAnd                = False
    )
    ToolSvc += TOPQElectronCCThinningTool
    thinningTools.append(TOPQElectronCCThinningTool)
    print TOPQname+".py", TOPQname+"ElectronCCThinningTool: ", TOPQElectronCCThinningTool


    #===============================
    # Calo Cluster Thinning (associated to photons)
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
        ApplyAnd                = False
    )
    ToolSvc += TOPQPhotonCCThinningTool
    thinningTools.append(TOPQPhotonCCThinningTool)
    print TOPQname+".py", TOPQname+"PhotonCCThinningTool: ", TOPQPhotonCCThinningTool


    #==============================
    # Photon thinning
    #==============================
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    pTPhotonVariableToCutOn = ".pt"
    photonColl = "Photons"
    TOPQPhotonThinning = DerivationFramework__GenericObjectThinning(
        name = TOPQname + photonColl + "Thinning_lowpTphotons",
        ThinningService         = TOPQThinningSvc,
        ContainerName    = photonColl,
        SelectionString  = photonColl + ".pt > 9.0*GeV"
    )
    ToolSvc += TOPQPhotonThinning
    thinningTools.append(TOPQPhotonThinning)
    print TOPQname+".py", TOPQname+photonColl+"Thinning: ", TOPQPhotonThinning


    #==============================================
    # TRUTH THINNING
    #==============================================
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource()=='geant4':
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning

        if TOPQname == 'TOPQ6':  # Specific requirements for TOPQ6: keep GEANT tracks
            TOPQTruthThinningTool = DerivationFramework__MenuTruthThinning(
                name                       = TOPQname + "TruthThinningTool",
                ThinningService            = TOPQThinningSvc,
                ParticlesKey               = "TruthParticles",
                VerticesKey                = "TruthVertices",
                EventsKey                  = "TruthEvents",
                WritePartons               = True,   # keep partons?  ###maybe set to FALSE?
                WriteHadrons               = False,  # keep hadrons?
                WriteBHadrons              = False,  # keep b-hadrons?
                WriteGeant                 = True,   # keep Geant particles?
                GeantPhotonPtThresh        = -1.0,   # Set to <0 to not write any Geant photons; otherwise write with a pT above threshold
                WriteTauHad                = True,   # keep hadronic taus?
                PartonPtThresh             = -1.0,   # write partons with pT aboe this threhold
                WriteBSM                   = True,   # keep BSM particles?
                WriteBSMProducts           = True,   # keep BSM particle decay products?
                WriteBosons                = True,   # keep bosons?
                WriteBosonProducts         = True,   # keep boson decay products?
                WriteTopAndDecays          = True,   # keep top partons and immediate decay products?
                WriteEverything            = False,  # keep everything?; overrides all other flags
                WriteAllLeptons            = True,   # keep absolutely all leptons?
                WriteLeptonsNotFromHadrons = False,  # keep leptons not from hadron decays
                WriteAllStable             = True,   # keep all stable particles?   ###maybe set to FALSE?
                WriteStatus3               = False,  # keep all particles with status code 3?
                WriteFirstN                = 10,     # keep first N particles in record
                PreserveDescendants        = False,  # keep descendants of retained particles?
                PreserveAncestors          = True,   # keep ancestors of retained particles?
                SimBarcodeOffset           = 200000, # barcode offset for simulation particles
                WritettHFHadrons           = True)   # keep tt+HF hadrons
        else:
            TOPQTruthThinningTool = DerivationFramework__MenuTruthThinning(
                name                       = TOPQname + "TruthThinningTool",
                ThinningService            = TOPQThinningSvc,
                ParticlesKey               = "TruthParticles",
                VerticesKey                = "TruthVertices",
                EventsKey                  = "TruthEvents",
                WritePartons               = True,   # keep partons?  ###maybe set to FALSE?
                WriteHadrons               = False,  # keep hadrons?
                WriteBHadrons              = False,  # keep b-hadrons?
                WriteGeant                 = False,  # keep Geant particles?
                GeantPhotonPtThresh        = -1.0,   # Set to <0 to not write any Geant photons; otherwise write with a pT above threshold
                WriteTauHad                = True,   # keep hadronic taus?
                PartonPtThresh             = -1.0,   # write partons with pT aboe this threhold
                WriteBSM                   = True,   # keep BSM particles?
                WriteBSMProducts           = True,   # keep BSM particle decay products?
                WriteBosons                = True,   # keep bosons?
                WriteBosonProducts         = True,   # keep boson decay products?
                WriteTopAndDecays          = True,   # keep top partons and immediate decay products?
                WriteEverything            = False,  # keep everything?; overrides all other flags
                WriteAllLeptons            = True,   # keep absolutely all leptons?
                WriteLeptonsNotFromHadrons = False,  # keep leptons not from hadron decays
                WriteAllStable             = True,   # keep all stable particles?   ###maybe set to FALSE?
                WriteStatus3               = False,  # keep all particles with status code 3?
                WriteFirstN                = 10,     # keep first N particles in record
                PreserveDescendants        = False,  # keep descendants of retained particles?
                PreserveAncestors          = True,   # keep ancestors of retained particles?
                SimBarcodeOffset           = 200000, # barcode offset for simulation particles
                WritettHFHadrons           = True)   # keep tt+HF hadrons


        ToolSvc += TOPQTruthThinningTool
        thinningTools.append(TOPQTruthThinningTool)
        print TOPQname+".py", TOPQname+"TruthThinningTool: ", TOPQTruthThinningTool

        if TOPQname == 'TOPQ5':
            # Only save truth informtion directly associated with Onia
            from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
            TOPQOniaTruthThinningTool = DerivationFramework__GenericTruthThinning(
                name                    = TOPQname + "OniaTruthThinningTool",
                ThinningService         = TOPQThinningSvc,
                ParticleSelectionString = "TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443 || TruthParticles.pdgId == 553 || TruthParticles.pdgId == 100553 || TruthParticles.pdgId == 200553",
                PreserveDescendants     = True,
                PreserveAncestors      = True
            )
            ToolSvc += TOPQOniaTruthThinningTool
            thinningTools.append(TOPQOniaTruthThinningTool)
            print TOPQname+".py", TOPQname+"OniaTruthThinningTool: ", TOPQOniaTruthThinningTool

        # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/GenericTruthThinning.cxx
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
        TOPQPhotonThinning = DerivationFramework__GenericTruthThinning(
            name                    = TOPQname + "PhotonThinning",
            ThinningService         = TOPQThinningSvc,
            ParticlesKey            = "TruthPhotons",
            ParticleSelectionString = "(TruthPhotons.classifierParticleOrigin != 42) || (TruthPhotons.pt > 20.0*GeV)"
        )
        ToolSvc += TOPQPhotonThinning
        thinningTools.append(TOPQPhotonThinning)
        print TOPQname+".py", TOPQname+"PhotonThinning: ", TOPQPhotonThinning

    return thinningTools
# end setup(TOPQname, TOPQThinningSvc, ToolSvc)
