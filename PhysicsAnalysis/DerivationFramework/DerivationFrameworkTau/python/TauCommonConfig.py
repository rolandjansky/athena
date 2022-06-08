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

# Low pT di-taus
def AddDiTauLowPtCfg(ConfigFlags, **kwargs):
    """Configure the low-pt di-tau building"""

    acc = ComponentAccumulator()
    prefix = kwargs['prefix']   

    from JetRecConfig.JetRecConfig import JetRecCfg
    from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    AntiKt10EMPFlow = AntiKt10LCTopo.clone(inputdef = cst.GPFlow)
    acc.merge(JetRecCfg(ConfigFlags,AntiKt10EMPFlow))

    from DiTauRec.DiTauBuilderConfig import DiTauBuilderLowPtCfg
    acc.merge(DiTauBuilderLowPtCfg(ConfigFlags, name=f"{prefix}_DiTauLowPtBuilder"))

    return(acc)


# TauWP decoration
def AddTauWPDecorationCfg(ConfigFlags, **kwargs):
    """Create the tau working point decorations"""

    kwargs.setdefault('evetoFixTag',None)
    prefix = kwargs['prefix']
    evetoFixTag = kwargs['evetoFixTag']
    acc = ComponentAccumulator()

    TauWPDecoratorWrapper = CompFactory.DerivationFramework.TauWPDecoratorWrapper
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel

    if (evetoFixTag=="v1"):
        evetoTauWPDecorator = CompFactory.TauWPDecorator(name = f"{prefix}TauWPDecoratorEleRNN",
                                                         flatteningFile1Prong = "rnneveto_mc16d_flat_1p_fix.root",
                                                         flatteningFile3Prong = "rnneveto_mc16d_flat_3p_fix.root",
                                                         DecorWPNames = [ "EleRNNLoose_"+evetoFixTag, "EleRNNMedium_"+evetoFixTag, "EleRNNTight_"+evetoFixTag ],
                                                         DecorWPCutEffs1P = [0.95, 0.90, 0.85],
                                                         DecorWPCutEffs3P = [0.98, 0.95, 0.90],
                                                         UseEleBDT = True,
                                                         ScoreName = "RNNEleScore",
                                                         NewScoreName = "RNNEleScoreSigTrans_"+evetoFixTag,
                                                         DefineWPs = True )

        acc.addPublicTool(evetoTauWPDecorator)
        evetoTauWPDecoratorWrapper = TauWPDecoratorWrapper(name               = f"{prefix}TauWPDecoratorEvetoWrapper",
                                                           TauContainerName   = "TauJets",
                                                           TauWPDecorator     = evetoTauWPDecorator)
        acc.addPublicTool(evetoTauWPDecoratorWrapper)
        acc.addEventAlgo(DerivationKernel(name              = f"{prefix}TauWPDecorator",
                                          AugmentationTools = [evetoTauWPDecoratorWrapper]))


    return(acc)      

