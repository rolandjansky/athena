# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def AddTauAugmentationCfg(ConfigFlags, doVeryLoose=False, doLoose=False, doMedium=False, doTight=False):

    acc = ComponentAccumulator()

    from DerivationFrameworkTools.DerivationFrameworkToolsConfig import AsgSelectionToolWrapperCfg
    from TauAnalysisTools.TauAnalysisToolsConfig import TauSelectionToolCfg

    TauAugmentationTools = []

    if doVeryLoose:
        TauSelectorVeryLoose = acc.popToolsAndMerge(TauSelectionToolCfg(ConfigFlags,
                                                                        name = 'TauSelectorLoose',
                                                                        ConfigPath = 'TauAnalysisAlgorithms/tau_selection_veryloose.conf'))
        
        acc.addPublicTool(TauSelectorVeryLoose)

        TauVeryLooseWrapper = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(ConfigFlags,
                                                                            name               = "TauVeryLooseWrapper",
                                                                            AsgSelectionTool   = TauSelectorVeryLoose,
                                                                            StoreGateEntryName = "DFTauVeryLoose",
                                                                            ContainerName      = "TauJets"))

        TauAugmentationTools.append(TauVeryLooseWrapper)


    if doLoose:
        TauSelectorLoose = acc.popToolsAndMerge(TauSelectionToolCfg(ConfigFlags,
                                                                    name = 'TauSelectorLoose',
                                                                    ConfigPath = 'TauAnalysisAlgorithms/tau_selection_loose.conf'))
        
        acc.addPublicTool(TauSelectorLoose)

        TauLooseWrapper = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(ConfigFlags, 
                                                                            name               = "TauLooseWrapper",
                                                                            AsgSelectionTool   = TauSelectorLoose,
                                                                            StoreGateEntryName = "DFTauLoose",
                                                                            ContainerName      = "TauJets"))

        TauAugmentationTools.append(TauLooseWrapper)


    if doMedium:
        TauSelectorMedium = acc.popToolsAndMerge(TauSelectionToolCfg(ConfigFlags,
                                                                    name = 'TauSelectorMedium',
                                                                    ConfigPath = 'TauAnalysisAlgorithms/tau_selection_medium.conf'))

        acc.addPublicTool(TauSelectorMedium)

        TauMediumWrapper = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(ConfigFlags,
                                                                            name               = "TauMediumWrapper",
                                                                            AsgSelectionTool   = TauSelectorMedium,
                                                                            StoreGateEntryName = "DFTauMedium",
                                                                            ContainerName      = "TauJets"))

        TauAugmentationTools.append(TauMediumWrapper)


    if doTight:
        TauSelectorTight = acc.popToolsAndMerge(TauSelectionToolCfg(ConfigFlags,
                                                                    name = 'TauSelectorTight',
                                                                    ConfigPath = 'TauAnalysisAlgorithms/tau_selection_medium.conf'))

        acc.addPublicTool(TauSelectorTight)

        TauTightWrapper = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(ConfigFlags,
                                                                            name               = "TauTightWrapper",
                                                                            AsgSelectionTool   = TauSelectorTight,
                                                                            StoreGateEntryName = "DFTauTight",
                                                                            ContainerName      = "TauJets"))

        TauAugmentationTools.append(TauTightWrapper)


    if TauAugmentationTools:
        CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
        acc.addEventAlgo(CommonAugmentation("TauAugmentationKernel", AugmentationTools = TauAugmentationTools))

    return acc
