# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# MuonsCommonConfig.py 
# Configures all tools needed for muon object selection and kernels
# used to write results into SG. 
# ComponentAccumulator version
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonsCommonCfg(ConfigFlags):
    """Main method configuring common muon augmentations"""
    
    acc = ComponentAccumulator()
    from DerivationFrameworkMuons import DFCommonMuonsConfig   
    DFCommonMuonsTrtCutOff = DFCommonMuonsConfig.TrtCutOff
   
    #====================================================================
    # MCP GROUP TOOLS 
    #====================================================================
   
    from DerivationFrameworkTools.DerivationFrameworkToolsConfig import AsgSelectionToolWrapperCfg
    DFCommonMuonToolWrapperTools = []
    
    ### IDHits
    # turn of the momentum correction which is not needed for IDHits cut and Preselection
    from MuonSelectorTools.MuonSelectorToolsConfig import MuonSelectionToolCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.Enums import LHCPeriod
    isRun3 = False
    if ConfigFlags.GeoModel.Run == LHCPeriod.Run3: isRun3 = True
    DFCommonMuonsSelector = acc.popToolsAndMerge(MuonSelectionToolCfg(
        ConfigFlags,
        name            = "DFCommonMuonsSelector",
        MaxEta          = 3.,
        MuQuality       = 3,
        IsRun3Geo       = isRun3,
        TurnOffMomCorr  = True)) 
    acc.addPublicTool(DFCommonMuonsSelector)
    if DFCommonMuonsTrtCutOff is not None: DFCommonMuonsSelector.TrtCutOff = DFCommonMuonsTrtCutOff
    DFCommonMuonToolWrapperIDCuts = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(
        ConfigFlags,
        name               = "DFCommonMuonToolWrapperIDCuts",
        AsgSelectionTool   = DFCommonMuonsSelector,
        CutType            = "IDHits",
        StoreGateEntryName = "DFCommonMuonPassIDCuts",
        ContainerName      = "Muons"))
    DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperIDCuts)
   
    DFCommonMuonToolWrapperPreselection = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(
        ConfigFlags,
        name               = "DFCommonMuonToolWrapperPreselection",
        AsgSelectionTool   = DFCommonMuonsSelector,
        CutType            = "Preselection",
        StoreGateEntryName = "DFCommonMuonPassPreselection",
        ContainerName      = "Muons"))
    DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperPreselection)
   
    #############
    #  Add tools
    #############
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
    acc.addEventAlgo(CommonAugmentation("DFCommonMuonsKernel",
                                        AugmentationTools = DFCommonMuonToolWrapperTools))

    from IsolationAlgs.DerivationTrackIsoConfig import DerivationTrackIsoCfg
    # A selection of WP is probably needed, as only a few variables are in CP content !
    #   maybe MUON derivations can add some other ones for studies
    #listofTTVAWP = [ 'Loose', 'Nominal', 'Tight',
    #                 'Prompt_D0Sig', 'Prompt_MaxWeight',
    #                 'Nonprompt_Hard_D0Sig',
    #                 'Nonprompt_Medium_D0Sig',
    #                 'Nonprompt_All_D0Sig',
    #                 'Nonprompt_Hard_MaxWeight',
    #                 'Nonprompt_Medium_MaxWeight',
    #                 'Nonprompt_All_MaxWeight' ]
    for WP in [ 'Nonprompt_All_MaxWeight' ]:
        acc.merge(DerivationTrackIsoCfg(ConfigFlags, WP = WP, object_type = ('Electrons', 'Muons')))

    return acc

