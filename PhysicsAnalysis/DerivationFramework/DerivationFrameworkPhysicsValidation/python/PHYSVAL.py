# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
#====================================================================
# DAOD_PHYSVAL.py
# This defines DAOD_PHYSVAL, an unskimmed DAOD format for 
# Run 3 validation. It contains the variables and objects needed for 
# most physics validation tasks in ATLAS.
# It requires the flag PHYSVAL in Derivation_tf.py   
#====================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import GSFTracksCPDetailedContent

# Main algorithm config
def PHYSVALKernelCfg(ConfigFlags, name='PHYSVALKernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel) for PHYSVAL"""
    acc = ComponentAccumulator()

    # Common augmentations
    from DerivationFrameworkPhys.PhysCommonConfig import PhysCommonAugmentationsCfg
    acc.merge(PhysCommonAugmentationsCfg(ConfigFlags, TriggerListsHelper = kwargs['TriggerListsHelper']))

    # Kernel algorithm
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name))       
    return acc


def PHYSVALCfg(ConfigFlags):

    acc = ComponentAccumulator()

    # Get the lists of triggers needed for trigger matching.
    # This is needed at this scope (for the slimming) and further down in the config chain
    # for actually configuring the matching, so we create it here and pass it down
    # TODO: this should ideally be called higher up to avoid it being run multiple times in a train
    from DerivationFrameworkPhys.TriggerListsHelper import TriggerListsHelper
    PHYSVALTriggerListsHelper = TriggerListsHelper()

    # Common augmentations
    acc.merge(PHYSVALKernelCfg(ConfigFlags, name="PHYSVALKernel", StreamName = 'StreamDAOD_PHYSVAL', TriggerListsHelper = PHYSVALTriggerListsHelper))

    # ============================
    # Define contents of the format
    # =============================
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    
    PHYSVALSlimmingHelper = SlimmingHelper("PHYSVALSlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    PHYSVALSlimmingHelper.SmartCollections = ["EventInfo",
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
                                              "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                              "AntiKtVR30Rmax4Rmin02PV0TrackJets"]
   
    PHYSVALSlimmingHelper.AllVariables =  ["EventInfo",
                                           "Electrons", "ForwardElectrons",
                                           "Photons",
                                           "Muons", "CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles",
                                           "MuonSpectrometerTrackParticles","MSOnlyExtrapolatedMuonTrackParticles","MuonSegments",
                                           "PrimaryVertices",
                                           "InDetTrackParticles","InDetForwardTrackParticles",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4EMPFlowJets",
                                           "BTagging_AntiKt4EMPFlow",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                           "BTagging_AntiKt4EMPFlowJFVtx", 
                                           "BTagging_AntiKt4EMPFlowJFVtxFlip", #Flip version of JetFitter
                                           "BTagging_AntiKt4EMPFlowSecVtx",
                                           "BTagging_AntiKt4EMPFlowSecVtxFlip", #Flip version of SV1
                                           "TauJets",
                                           "DiTauJets",
                                           "DiTauJetsLowPt",
                                           "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets","AntiKt10LCTopoJets","AntiKt4LCTopoJets","AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                           "TruthParticles", "TruthEvents", "TruthVertices", "MuonTruthParticles", "egammaTruthParticles",
                                           "MuonTruthSegments",
                                           "MET_Truth","MET_TruthRegions",
                                           "TruthElectrons","TruthMuons","TruthPhotons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson",
                                           "CaloCalTopoClusters", "EMOriginTopoClusters","LCOriginTopoClusters",
                                           "JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"]

    # TODO: for now we don't have a ConfigFlag for this so it is set to False
    AddPseudoTracks = False
    if AddPseudoTracks:
        PseudoTrackContainers = [
            "InDetPseudoTrackParticles",
            "InDetReplacedWithPseudoTrackParticles",
            "InDetReplacedWithPseudoFromBTrackParticles",
            "InDetReplacedWithPseudoNotFromBTrackParticles",
            "InDetPlusPseudoTrackParticles",
            "InDetPlusPseudoFromBTrackParticles",
            "InDetPlusPseudoNotFromBTrackParticles",
            "InDetNoFakesTrackParticles",
            "InDetNoFakesFromBTrackParticles",
            "InDetNoFakesNotFromBTrackParticles",
            "InDetSiSPSeededTracksParticles"]
        PHYSVALSlimmingHelper.SmartCollections += PseudoTrackContainers
        PHYSVALSlimmingHelper.AllVariables += PseudoTrackContainers
 
    excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
    StaticContent = []
    StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
    StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
    StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
    StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
    StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
    StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]
    StaticContent += ["xAOD::VertexAuxContainer#BTagging_AntiKt4EMPFlowSecVtxAux.-vxTrackAtVertex"]
    if ConfigFlags.BTagging.RunFlipTaggers is True:
        StaticContent += ["xAOD::VertexAuxContainer#BTagging_AntiKt4EMPFlowSecVtxFlipAux.-vxTrackAtVertex"]
 
    PHYSVALSlimmingHelper.StaticContent = StaticContent
    
    # Truth containers
    if ConfigFlags.Input.isMC:
        PHYSVALSlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                                    'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                    'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                                    'AntiKt10LCTopoJets':'xAOD::JetContainer', 'AntiKt10LCTopoJetsAux':'xAOD::JetAuxContainer',
                                                    'BTagging_AntiKtVR30Rmax4Rmin02Track':'xAOD::BTaggingContainer','BTagging_AntiKtVR30Rmax4Rmin02TrackAux':'xAOD::BTaggingAuxContainer',
                                                    'EMOriginTopoClusters':'xAOD::CaloClusterContainer', 'EMOriginTopoClustersAux':'xAOD::ShallowAuxContainer',
                                                    'LCOriginTopoClusters':'xAOD::CaloClusterContainer', 'LCOriginTopoClustersAux':'xAOD::ShallowAuxContainer',
                                                    'BTagging_AntiKt4EMPFlowJFVtx':'xAOD::BTagVertexContainer','BTagging_AntiKt4EMPFlowJFVtxAux':'xAOD::BTagVertexAuxContainer',
                                                    'BTagging_AntiKt4EMPFlowSecVtx':'xAOD::VertexContainer','BTagging_AntiKt4EMPFlowSecVtxAux':'xAOD::VertexAuxContainer',
                                                    'GlobalChargedParticleFlowObjects':'xAOD::FlowElementContainer','GlobalChargedParticleFlowObjectsAux':'xAOD::FlowElementAuxContainer',
                                                    'GlobalNeutralParticleFlowObjects':'xAOD::FlowElementContainer', 'GlobalNeutralParticleFlowObjectsAux':'xAOD::FlowElementAuxContainer',
                                                    'CHSGChargedParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSGChargedParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
                                                    'CHSGNeutralParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSGNeutralParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
                                                    'BTagging_AntiKt4EMPFlowJFVtxFlip':'xAOD::BTagVertexContainer','BTagging_AntiKt4EMPFlowJFVtxFlipAux':'xAOD::BTagVertexAuxContainer',#For Flip version of JetFitter
                                                    'BTagging_AntiKt4EMPFlowSecVtxFlip':'xAOD::VertexContainer','BTagging_AntiKt4EMPFlowSecVtxFlipAux':'xAOD::VertexAuxContainer'}
        from DerivationFrameworkMCTruth.MCTruthCommonConfig import addTruth3ContentToSlimmerTool
        addTruth3ContentToSlimmerTool(PHYSVALSlimmingHelper)
        PHYSVALSlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm','TruthPileupParticles','InTimeAntiKt4TruthJets','OutOfTimeAntiKt4TruthJets']
        # End of isMC clause       
 
    PHYSVALSlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                             "Electrons.TruthLink",
                                             "Muons.TruthLink",
                                             "Photons.TruthLink",
                                             "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                             "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                             "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                             "TruthPrimaryVertices.t.x.y.z",
                                             "TauNeutralParticleFlowObjects.pt.eta.phi.m.bdtPi0Score.nPi0Proto",
                                             "TauChargedParticleFlowObjects.pt.eta.phi.m.bdtPi0Score",
                                             "MET_Track.sumet"]
    PHYSVALSlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent

    # Trigger content
    PHYSVALSlimmingHelper.IncludeTriggerNavigation          = True
    PHYSVALSlimmingHelper.IncludeJetTriggerContent          = True
    PHYSVALSlimmingHelper.IncludeMuonTriggerContent         = True
    PHYSVALSlimmingHelper.IncludeEGammaTriggerContent       = True
    PHYSVALSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
    PHYSVALSlimmingHelper.IncludeTauTriggerContent          = True
    PHYSVALSlimmingHelper.IncludeEtMissTriggerContent       = True
    PHYSVALSlimmingHelper.IncludeBJetTriggerContent         = True
    PHYSVALSlimmingHelper.IncludeBPhysTriggerContent        = True
    PHYSVALSlimmingHelper.IncludeMinBiasTriggerContent      = True

    # Trigger matching
    # Run 2
    if ConfigFlags.Trigger.EDMVersion == 2:
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import AddRun2TriggerMatchingToSlimmingHelper
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSVALSlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_", 
                                         TriggerList = PHYSVALTriggerListsHelper.Run2TriggerNamesTau)
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSVALSlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_",
                                         TriggerList = PHYSVALTriggerListsHelper.Run2TriggerNamesNoTau)
    # Run 3
    if ConfigFlags.Trigger.EDMVersion == 3:
        from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import AddRun3TrigNavSlimmingCollectionsToSlimmingHelper
        AddRun3TrigNavSlimmingCollectionsToSlimmingHelper(PHYSVALSlimmingHelper)        
        # Run 2 is added here temporarily to allow testing/comparison/debugging
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import AddRun2TriggerMatchingToSlimmingHelper
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSVALSlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_", 
                                         TriggerList = PHYSVALTriggerListsHelper.Run3TriggerNamesTau)
        AddRun2TriggerMatchingToSlimmingHelper(SlimmingHelper = PHYSVALSlimmingHelper, 
                                         OutputContainerPrefix = "TrigMatch_",
                                         TriggerList = PHYSVALTriggerListsHelper.Run3TriggerNamesNoTau)


    # Output stream    
    PHYSVALItemList = PHYSVALSlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_PHYSVAL", ItemList=PHYSVALItemList, AcceptAlgs=["PHYSVALKernel"]))

    return acc

