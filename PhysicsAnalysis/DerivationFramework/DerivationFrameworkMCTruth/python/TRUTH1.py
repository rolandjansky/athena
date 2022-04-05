# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TRUTH1.py - format containing extended common ATLAS truth record

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TRUTH1Cfg(ConfigFlags):
    """Main config for TRUTH1"""
    acc = ComponentAccumulator()

    # Add translator from EVGEN input to xAOD-like truth
    # Add all the particle derivation tools
    # This sets up its own common kernel and adds the common tools to it
    from DerivationFrameworkMCTruth.MCTruthCommonConfig import AddStandardTruthContentsCfg
    acc.merge(AddStandardTruthContentsCfg(ConfigFlags))

    #==============================================================================
    # HEAVY FLAVOR DECORATIONS (ttbar)
    #==============================================================================
    from DerivationFrameworkMCTruth.HFHadronsCommonConfig import HFHadronsCommonCfg
    acc.merge(HFHadronsCommonCfg(ConfigFlags))

    #==============================================================================
    # Set up slimming content list
    #==============================================================================
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TRUTH1SlimmingHelper = SlimmingHelper("TRUTH1SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    TRUTH1SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                               'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                               'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthBottom':'xAOD::TruthParticleContainer','TruthBottomAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                               'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                               'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                               'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                               'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                               'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                               'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                               'AntiKtVR30Rmax4Rmin02TruthChargedJets':'xAOD::JetContainer','AntiKtVR30Rmax4Rmin02TruthChargedJetsAux':'xAOD::JetAuxContainer',
                                               'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                               'AntiKt10TruthJets':'xAOD::JetContainer','AntiKt10TruthJetsAux':'xAOD::JetAuxContainer',
                                               'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                               'TruthLabelBHadronsFinal':'xAOD::TruthParticleContainer', 'TruthLabelBHadronsInitial':'xAOD::TruthParticleContainer',
                                               'TruthLabelTQuarksFinal':'xAOD::TruthParticleContainer', 'TruthLabelBQuarksFinal':'xAOD::TruthParticleContainer', 'TruthLabelCQuarksFinal':'xAOD::TruthParticleContainer',                                               
                                               'TruthLabelCHadronsFinal':'xAOD::TruthParticleContainer', 'TruthLabelCHadronsInitial':'xAOD::TruthParticleContainer',
                                               'TruthLabelHBosons':'xAOD::TruthParticleContainer', 'TruthLabelWBosons':'xAOD::TruthParticleContainer', 'TruthLabelZBosons':'xAOD::TruthParticleContainer',
                                               'TruthLabelPartons':'xAOD::TruthParticleContainer', 'TruthLabelTausFinal':'xAOD::TruthParticleContainer'  
                                              }
    # Custom all variables -- won't be overridden by the call below, just added to. Full lists for completeness.
    TRUTH1SlimmingHelper.AllVariables = [ 'TruthEvents', 
                                          'TruthVertices',
                                          'TruthParticles',
                                          'TruthLabelBHadronsFinal',
                                          'TruthLabelBHadronsInitial',
                                          'TruthLabelBQuarksFinal',
                                          'TruthLabelCHadronsFinal',
                                          'TruthLabelCHadronsInitial',
                                          'TruthLabelCQuarksFinal',
                                          'TruthLabelHBosons',
                                          'TruthLabelPartons',
                                          'TruthLabelTQuarksFinal',
                                          'TruthLabelTausFinal',
                                          'TruthLabelWBosons',
                                          'TruthLabelZBosons']
    # Custom extra variables -- won't be overridden by the call below, just added to. Full lists for completeness.
    TRUTH1SlimmingHelper.ExtraVariables = ["AntiKtVR30Rmax4Rmin02TruthChargedJets.GhostHBosonsCount.GhostHBosonsPt.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.constituentLinks",
                                           "AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.constituentLinks",
                                           "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.constituentLinks.D2"]

    # Add standard content
    from DerivationFrameworkMCTruth.MCTruthCommonConfig import addTruth3ContentToSlimmerTool
    addTruth3ContentToSlimmerTool(TRUTH1SlimmingHelper)
    
    # Metadata
    TRUTH1MetaDataItems = [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] 

    # Generate item list and create stream 
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    TRUTH1ItemList = TRUTH1SlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TRUTH1", ItemList=TRUTH1ItemList, MetadataItemList=TRUTH1MetaDataItems))

    return acc
