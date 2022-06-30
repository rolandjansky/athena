# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# PhysCommonConfig
# Contains the configuration for the common physics containers/decorations used in analysis DAODs
# including PHYS(LITE)
# Actual configuration is subcontracted to other config files since some of them are very long

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod

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
        # Re-point links on reco objects
        acc.merge(AddMiniTruthCollectionLinksCfg(ConfigFlags))
        acc.merge(AddPVCollectionCfg(ConfigFlags))

    # InDet, Muon, Egamma common augmentations
    from DerivationFrameworkInDet.InDetCommonConfig import InDetCommonCfg
    from DerivationFrameworkMuons.MuonsCommonConfig import MuonsCommonCfg
    from DerivationFrameworkEGamma.EGammaCommonConfig import EGammaCommonCfg
    # TODO: need to find the new flags equivalent for the missing settings below, then we can
    # drop these kwargs and do everything via the ConfigFlags
    acc.merge(InDetCommonCfg(ConfigFlags,
                             DoVertexFinding = ConfigFlags.InDet.PriVertex.doVertexFinding,
                             AddPseudoTracks = ConfigFlags.InDet.Tracking.doPseudoTracking,
                             DecoLRTTTVA = False,
                             DoR3LargeD0 = ConfigFlags.InDet.Tracking.doR3LargeD0,
                             StoreSeparateLargeD0Container = ConfigFlags.InDet.Tracking.storeSeparateLargeD0Container,
                             MergeLRT = False)) 
    acc.merge(MuonsCommonCfg(ConfigFlags))
    acc.merge(EGammaCommonCfg(ConfigFlags))
    # Jets, di-taus, tau decorations, flavour tagging, MET association
    from DerivationFrameworkJetEtMiss.JetCommonConfig import JetCommonCfg
    from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollectionsCfg
    from DerivationFrameworkTau.TauCommonConfig import (AddDiTauLowPtCfg, AddTauWPDecorationCfg, AddMuonRemovalTauAODReRecoAlgCfg)
    from DerivationFrameworkJetEtMiss.METCommonConfig import METCommonCfg 
    acc.merge(JetCommonCfg(ConfigFlags))
    acc.merge(AddDiTauLowPtCfg(ConfigFlags, prefix = 'PhysCommon'))
    acc.merge(AddMuonRemovalTauAODReRecoAlgCfg(ConfigFlags, prefix = 'PhysCommon'))
    acc.merge(AddTauWPDecorationCfg(ConfigFlags, prefix = 'PhysCommon', evetoFixTag="v1"))
    FTagJetColl = ['AntiKt4EMPFlowJets','AntiKtVR30Rmax4Rmin02TrackJets']
    if ConfigFlags.GeoModel.Run >= LHCPeriod.Run4:
        FTagJetColl.append('AntiKt4EMTopoJets')
    acc.merge(FtagJetCollectionsCfg(ConfigFlags,FTagJetColl))
    acc.merge(METCommonCfg(ConfigFlags))

    # Trigger matching
    if ConfigFlags.Reco.EnableTrigger or ConfigFlags.Trigger.InputContainsConfigMetadata:
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import TriggerMatchingCommonRun2Cfg
        from DerivationFrameworkPhys.TriggerMatchingCommonConfig import TriggerMatchingCommonRun3Cfg
        # requires some wrangling due to the difference between run 2 and 3
        triggerListsHelper = kwargs['TriggerListsHelper']
        if ConfigFlags.Trigger.EDMVersion == 2:
            acc.merge(TriggerMatchingCommonRun2Cfg(ConfigFlags, 
                                                   name = "PhysCommonTrigMatchNoTau", 
                                                   OutputContainerPrefix = "TrigMatch_", 
                                                   TriggerList = triggerListsHelper.Run2TriggerNamesNoTau))
            acc.merge(TriggerMatchingCommonRun2Cfg(ConfigFlags, 
                                                   name = "PhysCommonTrigMatchTau", 
                                                   OutputContainerPrefix = "TrigMatch_", 
                                                   TriggerList = triggerListsHelper.Run2TriggerNamesTau, 
                                                   DRThreshold = 0.2))
        if ConfigFlags.Trigger.EDMVersion == 3:
            acc.merge(TriggerMatchingCommonRun3Cfg(ConfigFlags, TriggerList = triggerListsHelper.Run3TriggerNames))
            # This is here temporarily for testing/comparison/debugging purposes
            acc.merge(TriggerMatchingCommonRun2Cfg(ConfigFlags, 
                                                   name = "PhysCommonTrigMatchNoTau", 
                                                   OutputContainerPrefix = "TrigMatch_", 
                                                   TriggerList = triggerListsHelper.Run3TriggerNamesNoTau))
            acc.merge(TriggerMatchingCommonRun2Cfg(ConfigFlags, 
                                                   name = "PhysCommonTrigMatchTau", 
                                                   OutputContainerPrefix = "TrigMatch_", 
                                                   TriggerList = triggerListsHelper.Run3TriggerNamesTau, 
                                                   DRThreshold = 0.2))


    return acc

