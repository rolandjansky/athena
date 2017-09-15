# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# MuonsCommon.py 
# Schedules all tools needed for muon object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons import DFCommonMuonsConfig
DFCommonMuonsTrtCutOff = DFCommonMuonsConfig.TrtCutOff

#====================================================================
# MCP GROUP TOOLS 
#====================================================================

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
DFCommonMuonToolWrapperTools = []

### IDHits
DFCommonMuonsSelector = CP__MuonSelectionTool(name = "DFCommonMuonsSelector")
DFCommonMuonsSelector.MaxEta = 3.
DFCommonMuonsSelector.MuQuality = 3
# turn of the momentum correction which is not needed for IDHits cut and Preselection
DFCommonMuonsSelector.TurnOffMomCorr = True

if DFCommonMuonsTrtCutOff != None: DFCommonMuonsSelector.TrtCutOff = DFCommonMuonsTrtCutOff
ToolSvc += DFCommonMuonsSelector
print DFCommonMuonsSelector

DFCommonMuonToolWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapper",
                                                                        AsgSelectionTool = DFCommonMuonsSelector,
                                                                        CutType = "IDHits",
                                                                        StoreGateEntryName = "DFCommonGoodMuon",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapper
print DFCommonMuonToolWrapper
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapper)

### Preselection
DFCommonMuonsSelectorPreselection = CP__MuonSelectionTool(name = "DFCommonMuonsSelectorPreselection")
DFCommonMuonsSelectorPreselection.MaxEta = 3.
DFCommonMuonsSelectorPreselection.MuQuality = 3
# turn of the momentum correction which is not needed for IDHits cut and Preselection
DFCommonMuonsSelectorPreselection.TurnOffMomCorr = True

if DFCommonMuonsTrtCutOff != None: DFCommonMuonsSelectorPreselection.TrtCutOff = DFCommonMuonsTrtCutOff
ToolSvc += DFCommonMuonsSelectorPreselection
print DFCommonMuonsSelectorPreselection

DFCommonMuonToolWrapperPreselection = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapperPreselection",
                                                                        AsgSelectionTool = DFCommonMuonsSelectorPreselection,
                                                                        CutType = "Preselection",
                                                                        StoreGateEntryName = "DFCommonMuonsPreselection",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapperPreselection
print DFCommonMuonToolWrapperPreselection
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperPreselection)

### Decoration of the muon objects with the ID track covariances
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework_MuonIDCovMatrixDecorator
DFCommonMuonIDCovMatrixDecorator = DerivationFramework_MuonIDCovMatrixDecorator( name = "DFCommonMuonIDCovMatrixDecorator")
ToolSvc += DFCommonMuonIDCovMatrixDecorator
print DFCommonMuonIDCovMatrixDecorator
DFCommonMuonToolWrapperTools.append(DFCommonMuonIDCovMatrixDecorator)

#############
#  Add tools
#############
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel",
                                                                         AugmentationTools = DFCommonMuonToolWrapperTools
                                                                        )
