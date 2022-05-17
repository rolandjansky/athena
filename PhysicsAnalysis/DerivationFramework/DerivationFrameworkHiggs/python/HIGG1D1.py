# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
#====================================================================
# DAOD_HIGG1D1.py
# This defines DAOD_HIGG1D1, an unskimmed DAOD format for Run 3.
# It contains the variables and objects needed for the large majority 
# of physics analyses in ATLAS.
# It requires the flag HIGG1D1 in Derivation_tf.py   
#====================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Main algorithm config
def HIGG1D1KernelCfg(ConfigFlags, name='HIGG1D1Kernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel) for HIGG1D1"""
    acc = ComponentAccumulator()

    # Common augmentations
    from DerivationFrameworkPhys.PhysCommonConfig import PhysCommonAugmentationsCfg
    acc.merge(PhysCommonAugmentationsCfg(ConfigFlags))

    # Diphoton Vertex creation  
    from DerivationFrameworkHiggs.HIGG1D1CustomVertexConfig import ZeeVertexRefitterCfg, DiPhotonVertexCfg
    from DerivationFrameworkHiggs.HIGG1D1CustomJetsConfig import HIGG1D1CustomJetsCfg
    acc.merge(ZeeVertexRefitterCfg(ConfigFlags)) 
    acc.merge(DiPhotonVertexCfg(ConfigFlags))
    
    #CustomJetsConfig
    acc.merge(HIGG1D1CustomJetsCfg(ConfigFlags))

    # MET and Btagging
    #TODO

    # Thinning tools...
    from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleThinningCfg, MuonTrackParticleThinningCfg, TauTrackParticleThinningCfg, DiTauTrackParticleThinningCfg 
    #from DerivationFrameworkTools.DerivationFrameworkToolsConfig import GenericObjectThinningCfg

    # Inner detector group recommendations for indet tracks in analysis
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations
    HIGG1D1_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
    HIGG1D1TrackParticleThinningTool = acc.getPrimaryAndMerge(TrackParticleThinningCfg(
        ConfigFlags,
        name                    = "HIGG1D1TrackParticleThinningTool",
        StreamName              = kwargs['StreamName'], 
        SelectionString         = HIGG1D1_thinning_expression,
        InDetTrackParticlesKey  = "InDetTrackParticles"))
    
    # Include inner detector tracks associated with muons
    HIGG1D1MuonTPThinningTool = acc.getPrimaryAndMerge(MuonTrackParticleThinningCfg(
        ConfigFlags,
        name                    = "HIGG1D1MuonTPThinningTool",
        StreamName              = kwargs['StreamName'],
        MuonKey                 = "Muons",
        InDetTrackParticlesKey  = "InDetTrackParticles"))

    # Only keep tau tracks (and associated ID tracks) classified as charged tracks
    HIGG1D1TauTPThinningTool = acc.getPrimaryAndMerge(TauTrackParticleThinningCfg(
        ConfigFlags,
        name                   = "HIGG1D1TauTPThinningTool",
        StreamName             = kwargs['StreamName'],
        TauKey                 = "TauJets",
        InDetTrackParticlesKey = "InDetTrackParticles",
        DoTauTracksThinning    = True,
        TauTracksKey           = "TauTracks"))

    # ID tracks associated with high-pt di-tau
    HIGG1D1DiTauTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(
        ConfigFlags,
        name                    = "HIGG1D1DiTauTPThinningTool",
        StreamName              = kwargs['StreamName'],
        DiTauKey                = "DiTauJets",
        InDetTrackParticlesKey  = "InDetTrackParticles"))

    ## Low-pt di-tau thinning
    #HIGG1D1DiTauLowPtThinningTool = acc.getPrimaryAndMerge(GenericObjectThinningCfg(name            = "HIGG1D1DiTauLowPtThinningTool",
    #                                                                             StreamName      = kwargs['StreamName'],
    #                                                                             ContainerName   = "DiTauJetsLowPt",
    #                                                                             SelectionString = "DiTauJetsLowPt.nSubjets > 1"))
    #
    ## ID tracks associated with low-pt ditau
    #HIGG1D1DiTauLowPtTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(name                    = "HIGG1D1DiTauLowPtTPThinningTool",
    #                                                                                    StreamName              = kwargs['StreamName'],
    #                                                                                    DiTauKey                = "DiTauJetsLowPt",
    #                                                                                    InDetTrackParticlesKey  = "InDetTrackParticles",
    #                                                                                    SelectionString         = "DiTauJetsLowPt.nSubjets > 1"))

    # Finally the kernel itself
    thinningTools = [HIGG1D1TrackParticleThinningTool,
                     HIGG1D1MuonTPThinningTool,
                     HIGG1D1TauTPThinningTool,
                     HIGG1D1DiTauTPThinningTool]
                     #HIGG1D1DiTauLowPtThinningTool,
                     #HIGG1D1DiTauLowPtTPThinningTool ]
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, ThinningTools = thinningTools))       
    return acc


def HIGG1D1Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(HIGG1D1KernelCfg(ConfigFlags, name="HIGG1D1Kernel", StreamName = 'StreamDAOD_HIGG1D1'))
  
    # ============================
    # Define contents of the format
    # =============================
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    
    HIGG1D1SlimmingHelper = SlimmingHelper("HIGG1D1SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    HIGG1D1SlimmingHelper.SmartCollections = ["EventInfo",
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
    
    HIGG1D1SlimmingHelper.StaticContent = StaticContent
    
    # Trigger content
    HIGG1D1SlimmingHelper.IncludeTriggerNavigation = False
    HIGG1D1SlimmingHelper.IncludeJetTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeMuonTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeTauTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeEtMissTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeBJetTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeBPhysTriggerContent = False
    HIGG1D1SlimmingHelper.IncludeMinBiasTriggerContent = False

    # Truth containers
    if ConfigFlags.Input.isMC:
        HIGG1D1SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
        addTruth3ContentToSlimmerTool(HIGG1D1SlimmingHelper)
        HIGG1D1SlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm','TruthPileupParticles','InTimeAntiKt4TruthJets','OutOfTimeAntiKt4TruthJets']
        HIGG1D1SlimmingHelper.ExtraVariables += ["Electrons.TruthLink",
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
       
        HIGG1D1SlimmingHelper.AppendToDictionary.update({  "AntiKt4EMPFlowJetsCustomVtx": "xAOD::JetContainer", "AntiKt4EMPFlowCustomVtxJetsAux":"xAOD::JetAuxContainer",
          "HggPrimaryVertices":"xAOD::VertexContainer", "HggPrimaryVerticesAux":"xAOD::ShallowAuxContainer",
          "Kt4EMPFlowCustomVtxEventShape":"xAOD::EventShape", "Kt4EMPFlowCustomVtxEventShapeAux":"xAOD::EventShapeAuxInfo",
          "Kt4EMPFlowEventShape":"xAOD::EventShape", "Kt4EMPFlowEventShapeAux":"xAOD::EventShapeAuxInfo",
          "ZeeRefittedPrimaryVertices":"xAOD::VertexContainer","ZeeRefittedPrimaryVerticesAux":"xAOD:VertexAuxContainer"
        })

        HIGG1D1SlimmingHelper.AllVariables += ["HggPrimaryVertices","ZeeRefittedPrimaryVertices","AntiKt4EMPFlowCustomVtxJets","Kt4PFlowCustomVtxEventShape","Kt4EMPFlowEventShape"]

    
    # Output stream    
    HIGG1D1ItemList = HIGG1D1SlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_HIGG1D1", ItemList=HIGG1D1ItemList, AcceptAlgs=["HIGG1D1Kernel"]))

    return acc


# Add trigger matching
# Run 2
#PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(HIGG1D1SlimmingHelper)
#PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(HIGG1D1SlimmingHelper)
# Run 3
#from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import AddRun3TrigNavSlimmingCollectionsToEDM
#AddRun3TrigNavSlimmingCollectionsToEDM(HIGG1D1Stream)
