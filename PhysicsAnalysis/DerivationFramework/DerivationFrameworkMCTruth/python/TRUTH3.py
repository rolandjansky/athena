# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TRUTH3.py - format containing standard common ATLAS truth record

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TRUTH3Cfg(ConfigFlags):
    """Main config for TRUTH3"""
    acc = ComponentAccumulator()

    # Add translator from EVGEN input to xAOD-like truth
    # Add all the particle derivation tools
    # This sets up its own common kernel and adds the common tools to it
    from DerivationFrameworkMCTruth.MCTruthCommonConfig import AddStandardTruthContentsCfg
    acc.merge(AddStandardTruthContentsCfg(ConfigFlags))

    #==============================================================================
    # Set up slimming content list
    #==============================================================================
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TRUTH3SlimmingHelper = SlimmingHelper("TRUTH3SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)

    TRUTH3SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                           'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthSoftDropBeta100Zcut10Jets':'xAOD::JetContainer','AntiKt10TruthSoftDropBeta100Zcut10JetsAux':'xAOD::JetAuxContainer'
                                          }

    # Add standard content
    from DerivationFrameworkMCTruth.MCTruthCommonConfig import addTruth3ContentToSlimmerTool
    addTruth3ContentToSlimmerTool(TRUTH3SlimmingHelper)
    
    # Metadata
    TRUTH3MetaDataItems = [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] 

    # Generate item list and create stream 
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    TRUTH3ItemList = TRUTH3SlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TRUTH3", ItemList=TRUTH3ItemList, MetadataItemList=TRUTH3MetaDataItems))

    return acc
