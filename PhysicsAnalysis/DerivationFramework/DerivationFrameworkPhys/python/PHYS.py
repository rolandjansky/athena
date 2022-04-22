# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
#====================================================================
# DAOD_PHYS.py
# This defines DAOD_PHYS, an unskimmed DAOD format for Run 3.
# It contains the variables and objects needed for the large majority 
# of physics analyses in ATLAS.
# It requires the flag PHYS in Derivation_tf.py   
#====================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Main algorithm config
def PHYSKernelCfg(ConfigFlags, name='PHYSKernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel) for PHYS"""
    acc = ComponentAccumulator()

    # Common augmentations
    from DerivationFrameworkPhys.PhysCommonConfig import PhysCommonAugmentationsCfg
    acc.merge(PhysCommonAugmentationsCfg(ConfigFlags))

    # Thinning tools...
    from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleThinningCfg, MuonTrackParticleThinningCfg, TauTrackParticleThinningCfg, DiTauTrackParticleThinningCfg 
    #from DerivationFrameworkTools.DerivationFrameworkToolsConfig import GenericObjectThinningCfg

    # Inner detector group recommendations for indet tracks in analysis
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations
    PHYS_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
    PHYSTrackParticleThinningTool = acc.getPrimaryAndMerge(TrackParticleThinningCfg(
        ConfigFlags,
        name                    = "PHYSTrackParticleThinningTool",
        StreamName              = kwargs['StreamName'], 
        SelectionString         = PHYS_thinning_expression,
        InDetTrackParticlesKey  = "InDetTrackParticles"))
    
    # Include inner detector tracks associated with muons
    PHYSMuonTPThinningTool = acc.getPrimaryAndMerge(MuonTrackParticleThinningCfg(
        ConfigFlags,
        name                    = "PHYSMuonTPThinningTool",
        StreamName              = kwargs['StreamName'],
        MuonKey                 = "Muons",
        InDetTrackParticlesKey  = "InDetTrackParticles"))

    # Only keep tau tracks (and associated ID tracks) classified as charged tracks
    PHYSTauTPThinningTool = acc.getPrimaryAndMerge(TauTrackParticleThinningCfg(
        ConfigFlags,
        name                   = "PHYSTauTPThinningTool",
        StreamName             = kwargs['StreamName'],
        TauKey                 = "TauJets",
        InDetTrackParticlesKey = "InDetTrackParticles",
        DoTauTracksThinning    = True,
        TauTracksKey           = "TauTracks"))

    # ID tracks associated with high-pt di-tau
    PHYSDiTauTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(
        ConfigFlags,
        name                    = "PHYSDiTauTPThinningTool",
        StreamName              = kwargs['StreamName'],
        DiTauKey                = "DiTauJets",
        InDetTrackParticlesKey  = "InDetTrackParticles"))

    ## Low-pt di-tau thinning
    #PHYSDiTauLowPtThinningTool = acc.getPrimaryAndMerge(GenericObjectThinningCfg(name            = "PHYSDiTauLowPtThinningTool",
    #                                                                             StreamName      = kwargs['StreamName'],
    #                                                                             ContainerName   = "DiTauJetsLowPt",
    #                                                                             SelectionString = "DiTauJetsLowPt.nSubjets > 1"))
    #
    ## ID tracks associated with low-pt ditau
    #PHYSDiTauLowPtTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(name                    = "PHYSDiTauLowPtTPThinningTool",
    #                                                                                    StreamName              = kwargs['StreamName'],
    #                                                                                    DiTauKey                = "DiTauJetsLowPt",
    #                                                                                    InDetTrackParticlesKey  = "InDetTrackParticles",
    #                                                                                    SelectionString         = "DiTauJetsLowPt.nSubjets > 1"))

    # Finally the kernel itself
    thinningTools = [PHYSTrackParticleThinningTool,
                     PHYSMuonTPThinningTool,
                     PHYSTauTPThinningTool,
                     PHYSDiTauTPThinningTool]
                     #PHYSDiTauLowPtThinningTool,
                     #PHYSDiTauLowPtTPThinningTool ]
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, ThinningTools = thinningTools))       
    return acc


def PHYSCfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(PHYSKernelCfg(ConfigFlags, name="PHYSKernel", StreamName = 'StreamDAOD_PHYS'))
  
    # ============================
    # Define contents of the format
    # =============================
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    
    PHYSSlimmingHelper = SlimmingHelper("PHYSSlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    PHYSSlimmingHelper.SmartCollections = ["EventInfo",
                                           "Electrons",
                                           "Photons",
                                           "Muons",
                                           "PrimaryVertices",
                                           "InDetTrackParticles",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4EMPFlowJets",
                                           "BTagging_AntiKt4EMPFlow",
                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                           "MET_Baseline_AntiKt4EMTopo",
                                           "MET_Baseline_AntiKt4EMPFlow",
                                           "TauJets",
                                           "DiTauJets",
                                           "DiTauJetsLowPt",
                                           "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                           "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                          ]
    
    excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
    StaticContent = []
    StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
    StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
    StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
    StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
    StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
    StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]
    
    PHYSSlimmingHelper.StaticContent = StaticContent
    
    # Trigger content
    PHYSSlimmingHelper.IncludeTriggerNavigation = False
    PHYSSlimmingHelper.IncludeJetTriggerContent = False
    PHYSSlimmingHelper.IncludeMuonTriggerContent = False
    PHYSSlimmingHelper.IncludeEGammaTriggerContent = False
    PHYSSlimmingHelper.IncludeJetTauEtMissTriggerContent = False
    PHYSSlimmingHelper.IncludeTauTriggerContent = False
    PHYSSlimmingHelper.IncludeEtMissTriggerContent = False
    PHYSSlimmingHelper.IncludeBJetTriggerContent = False
    PHYSSlimmingHelper.IncludeBPhysTriggerContent = False
    PHYSSlimmingHelper.IncludeMinBiasTriggerContent = False

    # Truth containers
    if ConfigFlags.Input.isMC:
        PHYSSlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                                'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                                'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                                'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                                'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                'TruthCharm':'xAOD::TruthParticleContainer','TruthCharmAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer'
                                                }

        from DerivationFrameworkMCTruth.MCTruthCommonConfig import addTruth3ContentToSlimmerTool
        addTruth3ContentToSlimmerTool(PHYSSlimmingHelper)
        PHYSSlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm','TruthPileupParticles','InTimeAntiKt4TruthJets','OutOfTimeAntiKt4TruthJets']
        PHYSSlimmingHelper.ExtraVariables += ["Electrons.TruthLink",
                                              "Muons.TruthLink",
                                              "Photons.TruthLink",
                                              "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostBHadronsFinal.GhostCHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt",
                                              "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostBHadronsFinal.GhostCHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt",
                                              "TruthPrimaryVertices.t.x.y.z",
                                              "InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights.eProbabilityHT.numberOfTRTHits.numberOfTRTOutliers",
                                              "EventInfo.hardScatterVertexLink.timeStampNSOffset",
                                              "TauJets.dRmax.etOverPtLeadTrk",
                                              "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET.ex.ey",
                                              "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht.ex.ey"]
    
    # Output stream    
    PHYSItemList = PHYSSlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_PHYS", ItemList=PHYSItemList, AcceptAlgs=["PHYSKernel"]))

    return acc


# Add trigger matching
# Run 2
#PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(PHYSSlimmingHelper)
#PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(PHYSSlimmingHelper)
# Run 3
#from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import AddRun3TrigNavSlimmingCollectionsToEDM
#AddRun3TrigNavSlimmingCollectionsToEDM(PHYSStream)


