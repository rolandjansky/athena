# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

sPrefix = 'tauRec_'
bAODmode = False

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PanTauInformationStoreCfg(flags, infoHandler):
    result = ComponentAccumulator()
    PanTau__Tool_InformationStore = CompFactory.PanTau.Tool_InformationStore    

    # ===> Information Store
    python_Tool_InformationStore = PanTau__Tool_InformationStore("PanTau_InformationStore",
                                                                 Infos_Int       = infoHandler.m_Infos_Int,
                                                                 Infos_Double    = infoHandler.m_Infos_Double,
                                                                 Infos_VecDouble = infoHandler.m_Infos_VecDouble,
                                                                 Infos_String    = infoHandler.m_Infos_String,
                                                                 Infos_VecString = infoHandler.m_Infos_VecString)

    result.addPublicTool(python_Tool_InformationStore, True)
    return result

def PanTauInputConverterCfg(flags, infoHandler):
    result = ComponentAccumulator()
    PanTau__Tool_InputConverter = CompFactory.PanTau.Tool_InputConverter

    # ===> Input Converter
    python_Tool_InputConverter  = PanTau__Tool_InputConverter("PanTau_InputConverter",
                                                              Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler) ) )

    result.addPublicTool(python_Tool_InputConverter, True)
    return result

def PanTauConstGetterCfg(flags, infoHandler):
    result = ComponentAccumulator()
    PanTau__Tool_TauConstituentGetter = CompFactory.PanTau.Tool_TauConstituentGetter

    # ===> Tau Constituent Getter
    python_Tool_TauConstituentGetter = PanTau__Tool_TauConstituentGetter("PanTau_TauConstituentGetter",
                                                                         Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler) ),
                                                                         Tool_InputConverter   = result.getPrimaryAndMerge(PanTauInputConverterCfg(flags, infoHandler) ) )

    result.addPublicTool(python_Tool_TauConstituentGetter, True)
    return result

def PanTauConstSelectorCfg(flags, infoHandler):
    result = ComponentAccumulator()
    PanTau__Tool_TauConstituentSelector = CompFactory.PanTau.Tool_TauConstituentSelector

    # ===> Tau Constituent Selector
    python_Tool_TauConstituentSelector = PanTau__Tool_TauConstituentSelector("PanTau_TauConstituentSelector",
                                                                             Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler) ))

    result.addPublicTool(python_Tool_TauConstituentSelector, True)
    return result

def PanTauFeatureExtractorCfg(flags, infoHandler):
    result = ComponentAccumulator()
    PanTau__Tool_FeatureExtractor = CompFactory.PanTau.Tool_FeatureExtractor

    # ===> Tau Feature Extractor
    python_Tool_FeatureExtractor = PanTau__Tool_FeatureExtractor("PanTau_FeatureExtractor",
                                                                 Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler) ) )

    result.addPublicTool(python_Tool_FeatureExtractor, True)
    return result

def PanTauModeDiscr_1p0nv1p1n_Cfg(flags, infoHandler, curInAlg):
    result = ComponentAccumulator()
    PanTau__Tool_ModeDiscriminator = CompFactory.PanTau.Tool_ModeDiscriminator

    python_Tool_ModeDiscri_1p0n_vs_1p1n = PanTau__Tool_ModeDiscriminator("PanTau_ModeDiscri_1p0n_vs_1p1n_" + curInAlg,
                                                                         calibFolder             = flags.Tau.tauRecToolsCVMFSPath,
                                                                         Name_InputAlg           = curInAlg,
                                                                         Name_ModeCase           = "1p0n_vs_1p1n",
                                                                         Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler)) )

    result.addPublicTool(python_Tool_ModeDiscri_1p0n_vs_1p1n, True)
    return result

def PanTauModeDiscr_1p1nv1pXn_Cfg(flags, infoHandler, curInAlg):
    result = ComponentAccumulator()    
    PanTau__Tool_ModeDiscriminator = CompFactory.PanTau.Tool_ModeDiscriminator

    python_Tool_ModeDiscri_1p1n_vs_1pXn = PanTau__Tool_ModeDiscriminator("PanTau_ModeDiscri_1p1n_vs_1pXn_" + curInAlg,
                                                                         calibFolder             = flags.Tau.tauRecToolsCVMFSPath,
                                                                         Name_InputAlg           = curInAlg,
                                                                         Name_ModeCase           = "1p1n_vs_1pXn",
                                                                         Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler)) )

    result.addPublicTool(python_Tool_ModeDiscri_1p1n_vs_1pXn, True)
    return result

def PanTauModeDiscr_3p0nv3pXn_Cfg(flags, infoHandler, curInAlg):
    result = ComponentAccumulator()    
    PanTau__Tool_ModeDiscriminator = CompFactory.PanTau.Tool_ModeDiscriminator

    python_Tool_ModeDiscri_3p0n_vs_3pXn = PanTau__Tool_ModeDiscriminator("PanTau_ModeDiscri_3p0n_vs_3pXn_" + curInAlg,
                                                                         calibFolder             = flags.Tau.tauRecToolsCVMFSPath,
                                                                         Name_InputAlg           = curInAlg,
                                                                         Name_ModeCase           = "3p0n_vs_3pXn",
                                                                         Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler)) )

    result.addPublicTool(python_Tool_ModeDiscri_3p0n_vs_3pXn, True)
    return result

def PanTauDecayModeDetCfg(flags, infoHandler, curInAlg):
    result = ComponentAccumulator()    
    PanTau__Tool_DecayModeDeterminator = CompFactory.PanTau.Tool_DecayModeDeterminator

    # ===> Tau Decay Mode Determinator for current input alg
    Name_DecayModeDeterminator = "PanTau_DecayModeDeterminator_" + curInAlg
    python_Tool_DecayModeDeterminator = PanTau__Tool_DecayModeDeterminator(Name_DecayModeDeterminator,
                                                                           Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler)),
                                                                           Tool_ModeDiscriminator_1p0n_vs_1p1n = result.getPrimaryAndMerge(PanTauModeDiscr_1p0nv1p1n_Cfg(flags, infoHandler, curInAlg)),
                                                                           Tool_ModeDiscriminator_1p1n_vs_1pXn = result.getPrimaryAndMerge(PanTauModeDiscr_1p1nv1pXn_Cfg(flags, infoHandler, curInAlg)),
                                                                           Tool_ModeDiscriminator_3p0n_vs_3pXn = result.getPrimaryAndMerge(PanTauModeDiscr_3p0nv3pXn_Cfg(flags, infoHandler, curInAlg)) )
    
    result.addPublicTool(python_Tool_DecayModeDeterminator, True)
    return result

def PanTauDetailsArrangerCfg(flags, infoHandler):
    result = ComponentAccumulator()    
    PanTau__Tool_DetailsArranger = CompFactory.PanTau.Tool_DetailsArranger
    
    # ===> Details arranger tool
    python_Tool_DetailsArranger = PanTau__Tool_DetailsArranger("PanTau_DetailsArranger",
                                                                           Tool_InformationStore = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler)) )
    
    result.addPublicTool(python_Tool_DetailsArranger, True)
    return result

########################################################################
# PanTau
def PanTauCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'PanTau'
   
    # set up the job properties
    from PanTauAlgs.Class_InformationHandler import InformationHandler
    infoHandler = InformationHandler()

    curInAlg = 'CellBased'
    print("TopOptions_NewPanTau: Adding PanTau algorithms for input alg: " + curInAlg)

    PanTau__PanTauProcessor = CompFactory.PanTau.PanTauProcessor

    myPanTauProcessor = PanTau__PanTauProcessor(name = _name,
                                                Name_InputAlg               = curInAlg,
                                                Tool_InformationStore       = result.getPrimaryAndMerge(PanTauInformationStoreCfg(flags, infoHandler)),
                                                Tool_TauConstituentGetter   = result.getPrimaryAndMerge(PanTauConstGetterCfg(flags, infoHandler)),
                                                Tool_TauConstituentSelector = result.getPrimaryAndMerge(PanTauConstSelectorCfg(flags, infoHandler)),
                                                Tool_FeatureExtractor       = result.getPrimaryAndMerge(PanTauFeatureExtractorCfg(flags, infoHandler)),
                                                Tool_DecayModeDeterminator  = result.getPrimaryAndMerge(PanTauDecayModeDetCfg(flags, infoHandler, curInAlg)),
                                                Tool_DetailsArranger        = result.getPrimaryAndMerge(PanTauDetailsArrangerCfg(flags, infoHandler)) )

    result.setPrivateTools(myPanTauProcessor)
    return result
