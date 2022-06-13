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

# TauJets_MuonRM steering
def AddMuonRemovalTauAODReRecoAlgCfg(flags, **kwargs):
    """Configure the MuonRM AOD tau building"""

    prefix = kwargs['prefix']
    result=ComponentAccumulator()

    # get tools from holder
    import tauRec.TauToolHolder as tauTools
    tools_mod = []
    tools_mod.append( result.popToolsAndMerge(tauTools.TauAODMuonRemovalCfg(flags)) )
    tools_after = []
    tools_after.append( result.popToolsAndMerge(tauTools.TauVertexedClusterDecoratorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauTrackRNNClassifierCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.EnergyCalibrationLCCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauCommonCalcVarsCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauSubstructureCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.Pi0ClusterCreatorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.Pi0ClusterScalerCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.Pi0ScoreCalculatorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.Pi0SelectorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauVertexVariablesCfg(flags)) )
    import PanTauAlgs.JobOptions_Main_PanTau_New as pantau
    tools_after.append( result.popToolsAndMerge(pantau.PanTauCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauCombinedTESCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.MvaTESVariableDecoratorCfg(flags)) )
    tools_after[-1].EventShapeKey = ''
    tools_after.append( result.popToolsAndMerge(tauTools.MvaTESEvaluatorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauIDVarCalculatorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauJetRNNEvaluatorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauWPDecoratorJetRNNCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauEleRNNEvaluatorCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauWPDecoratorEleRNNCfg(flags)) )
    tools_after.append( result.popToolsAndMerge(tauTools.TauDecayModeNNClassifierCfg(flags)) )
    TauAODRunnerAlg=CompFactory.getComp("TauAODRunnerAlg")
    for tool in tools_mod:
        tool.inAOD = True
    for tool in tools_after:
        tool.inAOD = True
    myTauAODRunnerAlg = TauAODRunnerAlg(  
        name                           = prefix + "MuonRemovalTauAODReRecoAlg", 
        Key_tauOutputContainer         = "TauJets_MuonRM",
        Key_pi0OutputContainer         = "TauFinalPi0s_MuonRM",
        Key_neutralPFOOutputContainer  = "TauNeutralParticleFlowObjects_MuonRM",
        Key_chargedPFOOutputContainer  = "TauChargedParticleFlowObjects_MuonRM",
        Key_hadronicPFOOutputContainer = "TauHadronicParticleFlowObjects_MuonRM",
        Key_tauTrackOutputContainer    = "TauTracks_MuonRM",
        Key_vertexOutputContainer      = "TauSecondaryVertices_MuonRM",
        modificationTools              = tools_mod,
        officialTools                  = tools_after
    )
    result.addEventAlgo(myTauAODRunnerAlg)
    return result