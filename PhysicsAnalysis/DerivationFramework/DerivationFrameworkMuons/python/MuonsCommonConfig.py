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
    DFCommonMuonsSelector = acc.popToolsAndMerge(MuonSelectionToolCfg(
        ConfigFlags,
        name            = "DFCommonMuonsSelector",
        MaxEta          = 3.,
        MuQuality       = 3,
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
   
    # TODO: this goes down a rabbit hole of configs that need to be done in the ID
    # Not migrated yet
    #from IsolationAlgs.IsoUpdatedTrackConesConfig import AddUpdatedIsoTrackConesCfg
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Loose"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nominal"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Tight"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Prompt_D0Sig"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_Hard_D0Sig"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_Hard_D0Sig"))       
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_Medium_D0Sig"))    
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_All_D0Sig"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Prompt_MaxWeight"))          
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_Hard_MaxWeight"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_Medium_MaxWeight"))
    #acc.merge(AddUpdatedIsoTrackConesCfg(WP="Nonprompt_All_MaxWeight"))

    return acc

