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
    acc.merge(PhysCommonAugmentationsCfg(ConfigFlags, TriggerListsHelper = kwargs['TriggerListsHelper']))

    # Diphoton Vertex creation  
    from DerivationFrameworkHiggs.HIGG1D1CustomVertexConfig import ZeeVertexRefitterCfg, DiPhotonVertexCfg
    from DerivationFrameworkHiggs.HIGG1D1CustomJetsConfig import HIGG1D1CustomJetsCfg
    acc.merge(ZeeVertexRefitterCfg(ConfigFlags)) 
    acc.merge(DiPhotonVertexCfg(ConfigFlags))
    
    #CustomJetsConfig
    acc.merge(HIGG1D1CustomJetsCfg(ConfigFlags))
    
    from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollectionsCfg
    acc.merge(FtagJetCollectionsCfg(ConfigFlags,['AntiKt4EMPFlowCustomVtxJets'],['HggPrimaryVertices']))
    # 
    #TODO MET

    # Thinning tools...
    from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleThinningCfg, MuonTrackParticleThinningCfg, TauTrackParticleThinningCfg, DiTauTrackParticleThinningCfg  
    from DerivationFrameworkTools.DerivationFrameworkToolsConfig import GenericObjectThinningCfg

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
    HIGG1D1DiTauLowPtThinningTool = acc.getPrimaryAndMerge(GenericObjectThinningCfg(ConfigFlags,
                                                                                 name            = "PHYSDiTauLowPtThinningTool",
                                                                                 StreamName      = kwargs['StreamName'],
                                                                                 ContainerName   = "DiTauJetsLowPt",
                                                                                 SelectionString = "DiTauJetsLowPt.nSubjets > 1"))
    
    # ID tracks associated with low-pt ditau
    HIGG1D1DiTauLowPtTPThinningTool = acc.getPrimaryAndMerge(DiTauTrackParticleThinningCfg(ConfigFlags,
                                                                                        name                    = "PHYSDiTauLowPtTPThinningTool",
                                                                                        StreamName              = kwargs['StreamName'],
                                                                                        DiTauKey                = "DiTauJetsLowPt",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        SelectionString         = "DiTauJetsLowPt.nSubjets > 1"))
    
    
    # SkimmingTool
    from DerivationFrameworkHiggs.SkimmingToolHIGG1Config import SkimmingToolHIGG1Cfg
    
    # Requires something in this list of triggers
    SkipTriggerRequirement= ConfigFlags.Input.isMC and ConfigFlags.Beam.Energy==4000000.0 
    # 8 TeV MC does not have trigger information
    print( "HIGG1D1.py SkipTriggerRequirement", SkipTriggerRequirement)
    TriggerExp = []
    if not SkipTriggerRequirement:
        if ConfigFlags.Beam.Energy==4000000.0:
            #  8 TeV data
            TriggerExp               = ["EF_g35_loose_g25_loose"]
        if ConfigFlags.Beam.Energy==6500000.0:
            # 13 TeV MC
            TriggerExp               = ["HLT_2g50_loose_L12EM20VH","HLT_2g25_loose_g15_loose","HLT_g35_medium_g25_medium_L12EM20VH","HLT_2g25_tight_L12EM20VH","HLT_2g22_tight_L12EM15VHI","HLT_g35_loose_g25_loose","HLT_g35_medium_g25_medium","HLT_2g50_loose","HLT_2g20_tight","HLT_2g22_tight","HLT_2g20_tight_icalovloose_L12EM15VHI","HLT_2g20_tight_icalotight_L12EM15VHI","HLT_2g22_tight_L12EM15VHI","HLT_2g22_tight_icalovloose_L12EM15VHI","HLT_2g22_tight_icalotight_L12EM15VHI","HLT_2g22_tight_icalovloose","HLT_2g25_tight_L12EM20VH","HLT_2g20_loose","HLT_2g20_loose_L12EM15","HLT_g35_medium_g25_medium","HLT_g35_medium_g25_medium_L12EM15VH","HLT_g35_loose_g25_loose","HLT_g35_loose_g25_loose_L12EM15VH", "HLT_2g20_loose_g15_loose", "HLT_3g20_loose", "HLT_3g15_loose", "HLT_2g6_tight_icalotight_L1J100", "HLT_2g6_loose_L1J100", "HLT_2g6_tight_icalotight_L1J50", "HLT_2g6_loose_L1J50","HLT_g120_loose","HLT_g140_loose"]

    skimmingTool = acc.popToolsAndMerge( SkimmingToolHIGG1Cfg(ConfigFlags,RequireTrigger=not SkipTriggerRequirement,Triggers=TriggerExp) )
    acc.addPublicTool(skimmingTool)


    # Finally the kernel itself
    thinningTools = [HIGG1D1TrackParticleThinningTool,
                     HIGG1D1MuonTPThinningTool,
                     HIGG1D1TauTPThinningTool,
                     HIGG1D1DiTauTPThinningTool,
                     HIGG1D1DiTauLowPtThinningTool,
                     HIGG1D1DiTauLowPtTPThinningTool ]
    
    #====================================================================
    # Max Cell sum decoration tool
    #====================================================================
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    acc.merge(LArOnOffIdMappingCfg(ConfigFlags))
    
    augmentationTools = [] 
    from DerivationFrameworkCalo.DerivationFrameworkCaloConfig import MaxCellDecoratorCfg
    MaxCellDecorator = acc.popToolsAndMerge(MaxCellDecoratorCfg(ConfigFlags))
    acc.addPublicTool(MaxCellDecorator)
    augmentationTools.append(MaxCellDecorator)
    #====================================================================
    # Gain and cluster energies per layer decoration tool
    #====================================================================

    from DerivationFrameworkCalo.DerivationFrameworkCaloConfig import GainDecoratorCfg, ClusterEnergyPerLayerDecoratorCfg
    GainDecoratorTool = acc.popToolsAndMerge(GainDecoratorCfg(ConfigFlags))
    acc.addPublicTool(GainDecoratorTool)
    augmentationTools.append(GainDecoratorTool)


    cluster_sizes = (3,5), (5,7), (7,7), (7,11)
    for neta, nphi in cluster_sizes:
        cename = "ClusterEnergyPerLayerDecorator_%sx%s" % (neta, nphi)
        ClusterEnergyPerLayerDecorator = acc.popToolsAndMerge( ClusterEnergyPerLayerDecoratorCfg(ConfigFlags, neta = neta, nphi=nphi, name=cename ))
        acc.addPublicTool(ClusterEnergyPerLayerDecorator)
        augmentationTools.append(ClusterEnergyPerLayerDecorator)
    
    
    
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, 
                                      SkimmingTools = [skimmingTool],
                                      ThinningTools = thinningTools,
                                      AugmentationTools = augmentationTools))     
    return acc


def HIGG1D1Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    
    from DerivationFrameworkPhys.TriggerListsHelper import TriggerListsHelper
    HIGG1D1TriggerListsHelper = TriggerListsHelper()
    
    acc.merge(HIGG1D1KernelCfg(ConfigFlags, name="HIGG1D1Kernel", StreamName = 'StreamDAOD_HIGG1D1', TriggerListsHelper = HIGG1D1TriggerListsHelper))


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

    #  Additional content for HIGG1D1   
    HIGG1D1SlimmingHelper.AppendToDictionary.update({  "AntiKt4EMPFlowCustomVtxJets": "xAOD::JetContainer", "AntiKt4EMPFlowCustomVtxJetsAux":"xAOD::JetAuxContainer",
          "HggPrimaryVertices":"xAOD::VertexContainer", "HggPrimaryVerticesAux":"xAOD::ShallowAuxContainer",
          "Kt4EMPFlowCustomVtxEventShape":"xAOD::EventShape", "Kt4EMPFlowCustomVtxEventShapeAux":"xAOD::EventShapeAuxInfo",
          "Kt4EMPFlowEventShape":"xAOD::EventShape", "Kt4EMPFlowEventShapeAux":"xAOD::EventShapeAuxInfo",
          "ZeeRefittedPrimaryVertices":"xAOD::VertexContainer","ZeeRefittedPrimaryVerticesAux":"xAOD:VertexAuxContainer",
          "AFPSiHitContainer":"xAOD::AFPSiHitContainer","AFPSiHitContainerAux":"xAOD::AFPSiHitAuxContainer",
          "AFPToFHitContainer":"xAOD::AFPToFHitContainer","AFPToFHitContainerAux":"xAOD::AFPToFHitAuxContainer",
          "BTagging_AntiKt4EMPFlowCustomVtx":"xAOD::BTaggingContainer","BTagging_AntiKt4EMPFlowCustomVtxAux":"xAOD:BTaggingAuxContainer"
        })

    HIGG1D1SlimmingHelper.AllVariables += ["HggPrimaryVertices","ZeeRefittedPrimaryVertices","AntiKt4EMPFlowCustomVtxJets","Kt4EMPFlowCustomVtxEventShape","Kt4EMPFlowEventShape"]
    
    # Add AFP information
    HIGG1D1SlimmingHelper.AllVariables += ["AFPSiHitContainer","AFPToFHitContainer"]

    # Add Btagging information
    from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent,BTaggingXbbContent
    HIGG1D1SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMPFlowCustomVtxJets")
    HIGG1D1SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMPFlowJets")
    HIGG1D1SlimmingHelper.ExtraVariables += BTaggingXbbContent("AntiKt4EMPFlowCustomVtxJets")
    HIGG1D1SlimmingHelper.ExtraVariables += BTaggingXbbContent("AntiKt4EMPFlowJets")

    # is this really needed given Photons are in the AllVariables list ?
    from DerivationFrameworkEGamma.PhotonsCPDetailedContent import PhotonsCPDetailedContent
    HIGG1D1SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent


    # Add the variables for Gain adn Cluster energy  --  need to get the tools first to use existing functions 
    from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import getGainDecorations, getClusterEnergyPerLayerDecorations  
    GainDecoratorTool = None
    ClusterEnergyPerLayerDecorators = []  
    for toolStr in acc.getEventAlgo("HIGG1D1Kernel").AugmentationTools:
        toolStr  = f"{toolStr}"
        splitStr = toolStr.split('/')
        tool =  acc.getPublicTool(splitStr[1])
        if splitStr[0]== "DerivationFramework::GainDecorator":
            GainDecoratorTool = tool
        elif splitStr[0] ==  "DerivationFramework::ClusterEnergyPerLayerDecorator":
            ClusterEnergyPerLayerDecorators.append( tool )

    
    if GainDecoratorTool : 
        HIGG1D1SlimmingHelper.ExtraVariables.extend( getGainDecorations(GainDecoratorTool) )
    for tool in ClusterEnergyPerLayerDecorators:
        HIGG1D1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )



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

    # Trigger matching
    # Run 2
    if ConfigFlags.Trigger.EDMVersion == 2:
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import AddRun2TriggerMatchingToSlimmingHelper
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = HIGG1D1SlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_", 
                                         TriggerList = HIGG1D1TriggerListsHelper.Run2TriggerNamesTau)
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = HIGG1D1SlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_",
                                         TriggerList = HIGG1D1TriggerListsHelper.Run2TriggerNamesNoTau)
    # Run 3
    if ConfigFlags.Trigger.EDMVersion == 3:
        from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import AddRun3TrigNavSlimmingCollectionsToSlimmingHelper
        AddRun3TrigNavSlimmingCollectionsToSlimmingHelper(HIGG1D1SlimmingHelper)        
        # Run 2 is added here temporarily to allow testing/comparison/debugging
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import AddRun2TriggerMatchingToSlimmingHelper
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = HIGG1D1SlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_", 
                                         TriggerList = HIGG1D1TriggerListsHelper.Run3TriggerNamesTau)
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = HIGG1D1SlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_",
                                         TriggerList = HIGG1D1TriggerListsHelper.Run3TriggerNamesNoTau)
    
    # Output stream    
    HIGG1D1ItemList = HIGG1D1SlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_HIGG1D1", ItemList=HIGG1D1ItemList, AcceptAlgs=["HIGG1D1Kernel"]))

    return acc



