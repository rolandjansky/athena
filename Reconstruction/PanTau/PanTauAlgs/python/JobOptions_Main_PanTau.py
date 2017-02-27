# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

cached_instances = {}

sPrefix = 'tauRec_'
bAODmode = False

from tauRec.tauRecFlags import jobproperties as config_TauRec

########################################################################
# PanTau
def getPanTau():
    _name = sPrefix + 'PanTau'
   
    if _name in cached_instances:
        return cached_instances[_name]
   
    # set message limit
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
    svcMgr.MessageSvc.defaultLimit = 9999999 


    # set up the job properties
    from PanTauAlgs.Class_InformationHandler import InformationHandler
    infoHandler = InformationHandler()


    # copy the configuration from cellbased if requested
    from PanTauAlgs.Config_PanTau import config_PanTau
    #if config_PanTau.UseDefaultCellBasedConfig == True:
    
        # pi0-BDT cuts
        #config_PanTau.CellBased_EtaBinned_Pi0MVACut_1prong.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0MVACuts_1prong() )
        #config_PanTau.CellBased_EtaBinned_Pi0MVACut_3prong.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0MVACuts_mprong() )
    
        # Et cuts
        #config_PanTau.TauConstituents_Selection_Neutral_EtaBinned_EtCut.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0EtCuts() )
        #config_PanTau.TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0EtCuts() )


    # Create tools
    # ==================================================================
    from AthenaCommon.AppMgr import ToolSvc
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_InformationStore
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_InputConverter
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_TauConstituentGetter
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_TauConstituentSelector
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_FeatureExtractor
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_ModeDiscriminator
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_DecayModeDeterminator
    from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_DetailsArranger
    from PanTauAlgs.PanTauAlgsConf import PanTau__PanTauProcessor

    # ===> Information Store
    python_Tool_InformationStore = PanTau__Tool_InformationStore(   "PanTau_InformationStore",
                                                                    Infos_Int       = infoHandler.m_Infos_Int,
                                                                    Infos_Double    = infoHandler.m_Infos_Double,
                                                                    Infos_VecDouble = infoHandler.m_Infos_VecDouble,
                                                                    Infos_String    = infoHandler.m_Infos_String,
                                                                    Infos_VecString = infoHandler.m_Infos_VecString)
    python_Tool_InformationStore.OutputLevel = INFO
    ToolSvc += python_Tool_InformationStore
    
    
    # ===> Input Converter
    python_Tool_InputConverter  = PanTau__Tool_InputConverter("PanTau_InputConverter",
                                                              Tool_InformationStore = python_Tool_InformationStore)
    python_Tool_InputConverter.OutputLevel = INFO
    ToolSvc += python_Tool_InputConverter
    
    # ===> Tau Constituent Getter
    python_Tool_TauConstituentGetter = PanTau__Tool_TauConstituentGetter(   "PanTau_TauConstituentGetter",
                                                                            Tool_InformationStore = python_Tool_InformationStore,
                                                                            Tool_InputConverter   = python_Tool_InputConverter)
    python_Tool_TauConstituentGetter.OutputLevel = INFO
    ToolSvc += python_Tool_TauConstituentGetter
    
    # ===> Tau Constituent Selector
    python_Tool_TauConstituentSelector = PanTau__Tool_TauConstituentSelector( "PanTau_TauConstituentSelector",
                                                                              Tool_InformationStore = python_Tool_InformationStore)
    python_Tool_TauConstituentSelector.OutputLevel = INFO
    ToolSvc += python_Tool_TauConstituentSelector
    
    # ===> Tau Feature Extractor
    python_Tool_FeatureExtractor = PanTau__Tool_FeatureExtractor(   "PanTau_FeatureExtractor",
                                                                    Tool_InformationStore           = python_Tool_InformationStore
                                                                    )
    python_Tool_FeatureExtractor.OutputLevel = INFO
    ToolSvc += python_Tool_FeatureExtractor
    
    # ===> Details arranger tool
    python_Tool_DetailsArranger = PanTau__Tool_DetailsArranger( "PanTau_DetailsArranger",
                                                                Tool_InformationStore           = python_Tool_InformationStore)
    python_Tool_DetailsArranger.OutputLevel = INFO
    ToolSvc += python_Tool_DetailsArranger



    from PanTauAlgs.Config_PanTau import config_PanTau
    List_InputAlgs      = config_PanTau.Names_InputAlgorithms()

    #for curInAlg in List_InputAlgs:
    curInAlg = 'CellBased'

    print("TopOptions_NewPanTau: Adding PanTau algorithms for input alg: " + curInAlg)
    
    
    # ===> create the discri tools for this input algorithm
    from tauRec.tauRecFlags import tauFlags
    tauRecToolsCalibPath = tauFlags.tauRecToolsCVMFSPath()
    python_Tool_ModeDiscri_1p0n_vs_1p1n = PanTau__Tool_ModeDiscriminator(   "PanTau_ModeDiscri_1p0n_vs_1p1n_" + curInAlg,
                                                                            calibFolder             = tauRecToolsCalibPath,
                                                                            Name_InputAlg           = curInAlg,
                                                                            Name_ModeCase           = "1p0n_vs_1p1n",
                                                                            Tool_InformationStore   = python_Tool_InformationStore)
    python_Tool_ModeDiscri_1p0n_vs_1p1n.OutputLevel = INFO
    ToolSvc += python_Tool_ModeDiscri_1p0n_vs_1p1n

    python_Tool_ModeDiscri_1p1n_vs_1pXn = PanTau__Tool_ModeDiscriminator(   "PanTau_ModeDiscri_1p1n_vs_1pXn_" + curInAlg,
                                                                            calibFolder             = tauRecToolsCalibPath,
                                                                            Name_InputAlg           = curInAlg,
                                                                            Name_ModeCase           = "1p1n_vs_1pXn",
                                                                            Tool_InformationStore   = python_Tool_InformationStore)
    python_Tool_ModeDiscri_1p1n_vs_1pXn.OutputLevel = INFO
    ToolSvc += python_Tool_ModeDiscri_1p1n_vs_1pXn

    python_Tool_ModeDiscri_3p0n_vs_3pXn = PanTau__Tool_ModeDiscriminator(   "PanTau_ModeDiscri_3p0n_vs_3pXn_" + curInAlg,
                                                                            calibFolder             = tauRecToolsCalibPath,
                                                                            Name_InputAlg           = curInAlg,
                                                                            Name_ModeCase           = "3p0n_vs_3pXn",
                                                                            Tool_InformationStore   = python_Tool_InformationStore)
    python_Tool_ModeDiscri_3p0n_vs_3pXn.OutputLevel = INFO
    ToolSvc += python_Tool_ModeDiscri_3p0n_vs_3pXn


    # ===> Tau Decay Mode Determinator for current input alg
    Name_DecayModeDeterminator = "PanTau_DecayModeDeterminator_" + curInAlg
    python_Tool_DecayModeDeterminator = PanTau__Tool_DecayModeDeterminator( Name_DecayModeDeterminator,
                                                                            Tool_InformationStore               = python_Tool_InformationStore,
                                                                            Tool_ModeDiscriminator_1p0n_vs_1p1n = python_Tool_ModeDiscri_1p0n_vs_1p1n,
                                                                            Tool_ModeDiscriminator_1p1n_vs_1pXn = python_Tool_ModeDiscri_1p1n_vs_1pXn,
                                                                            Tool_ModeDiscriminator_3p0n_vs_3pXn = python_Tool_ModeDiscri_3p0n_vs_3pXn)
    python_Tool_DecayModeDeterminator.OutputLevel = INFO
    ToolSvc += python_Tool_DecayModeDeterminator




    from PanTauAlgs.PanTauAlgsConf import PanTau__PanTauProcessor
    PanTau__PanTauProcessor = PanTau__PanTauProcessor(  name = _name, 
                                                        Name_InputAlg               = curInAlg,
                                                        Tool_InformationStore       = python_Tool_InformationStore,
                                                        Tool_TauConstituentGetter   = python_Tool_TauConstituentGetter,
                                                        Tool_TauConstituentSelector = python_Tool_TauConstituentSelector,
                                                        Tool_FeatureExtractor       = python_Tool_FeatureExtractor,
                                                        Tool_DecayModeDeterminator  = python_Tool_DecayModeDeterminator,
                                                        Tool_DetailsArranger        = python_Tool_DetailsArranger)
    PanTau__PanTauProcessor.OutputLevel = INFO


    cached_instances[_name] = PanTau__PanTauProcessor
    return PanTau__PanTauProcessor












