# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# PhysCommonConfig
# Contains the configuration for the common physics containers/decorations used in analysis DAODs
# including PHYS(LITE)
# Actual configuration is subcontracted to other config files since some of them are very long

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysCommonAugmentationsCfg(ConfigFlags,**kwargs):
    """Configure the common augmentation"""
    acc = ComponentAccumulator()

    # MC truth
    if ConfigFlags.Input.isMC:
        from DerivationFrameworkMCTruth.MCTruthCommonConfig import (
            AddStandardTruthContentsCfg,
            AddHFAndDownstreamParticlesCfg,
            AddMiniTruthCollectionLinksCfg,
            AddPVCollectionCfg,
            AddTruthCollectionNavigationDecorationsCfg)
        from DerivationFrameworkMCTruth.TruthDerivationToolsConfig import TruthCollectionMakerCfg
        PhysCommonTruthCharmTool = acc.getPrimaryAndMerge(TruthCollectionMakerCfg(
            name                    = "PhysCommonTruthCharmTool",
            NewCollectionName       = "TruthCharm",
            KeepNavigationInfo      = False,
            ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
            Do_Compress             = True)) 
        CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
        acc.addEventAlgo(CommonAugmentation("PhysCommonTruthCharmKernel",AugmentationTools=[PhysCommonTruthCharmTool]))
        acc.merge(AddHFAndDownstreamParticlesCfg(ConfigFlags))
        acc.merge(AddStandardTruthContentsCfg(ConfigFlags))
        acc.merge(AddTruthCollectionNavigationDecorationsCfg(
            TruthCollections=["TruthElectrons", 
                              "TruthMuons", 
                              "TruthPhotons", 
                              "TruthTaus", 
                              "TruthNeutrinos", 
                              "TruthBSM", 
                              "TruthBottom", 
                              "TruthTop", 
                              "TruthBoson",
                              "TruthCharm",
                              "TruthHFWithDecayParticles"],
            prefix = 'PHYS_'))
        #acc.getEventAlgo("MCTruthNavigationDecoratorKernel").AugmentationTools = [PhysCommonTruthNavigationDecorator]
        # Re-point links on reco objects
        acc.merge(AddMiniTruthCollectionLinksCfg(ConfigFlags))
        acc.merge(AddPVCollectionCfg(ConfigFlags))
        # TODO: not at all clear what is supposed to be achieved by the next line or how to do it in the CA 
        # Set appropriate truth jet collection for tau truth matching
        # ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"



    # InDet, Muon and Egamma common augmentations
    from DerivationFrameworkInDet.InDetCommonConfig import InDetCommonCfg
    from DerivationFrameworkMuons.MuonsCommonConfig import MuonsCommonCfg
    from DerivationFrameworkEGamma.EGammaCommonConfig import EGammaCommonCfg
    from DerivationFrameworkJetEtMiss.JetCommonConfig import JetCommonCfg
    # TODO: need to find the new flags equivalent for the missing settings below, then we can
    # drop these kwargs and do everything via the ConfigFlags
    acc.merge(InDetCommonCfg(ConfigFlags,
                             DoVertexFinding = ConfigFlags.InDet.PriVertex.doVertexFinding,
                             AddPseudoTracks = False,
                             DecoLRTTTVA = False,
                             DoR3LargeD0 = ConfigFlags.InDet.Tracking.doR3LargeD0,
                             StoreSeparateLargeD0Container = ConfigFlags.InDet.Tracking.storeSeparateLargeD0Container,
                             MergeLRT = False)) 
    acc.merge(MuonsCommonCfg(ConfigFlags))
    acc.merge(EGammaCommonCfg(ConfigFlags))
    acc.merge(JetCommonCfg(ConfigFlags))

    # Jets...
    # Tau...
    # Flavour tagging...
    # Truth...
    # Trigger

    return acc

