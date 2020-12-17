# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# MuonsCommon.py 
# Schedules all tools needed for muon object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from __future__ import print_function

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from DerivationFrameworkMuons import DFCommonMuonsConfig
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr 
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

if DFCommonMuonsTrtCutOff is not None: DFCommonMuonsSelector.TrtCutOff = DFCommonMuonsTrtCutOff
ToolSvc += DFCommonMuonsSelector
print (DFCommonMuonsSelector)

DFCommonMuonToolWrapperIDCuts = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapperIDCuts",
                                                                        AsgSelectionTool = DFCommonMuonsSelector,
                                                                        CutType = "IDHits",
                                                                        StoreGateEntryName = "DFCommonMuonPassIDCuts",
                                                                        ContainerName = "Muons")
#preselection
ToolSvc += DFCommonMuonToolWrapperIDCuts
print (DFCommonMuonToolWrapperIDCuts)
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperIDCuts)

DFCommonMuonToolWrapperPreselection = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapperPreselection",
                                                                        AsgSelectionTool = DFCommonMuonsSelector,
                                                                        CutType = "Preselection",
                                                                        StoreGateEntryName = "DFCommonMuonPassPreselection",
                                                                        ContainerName = "Muons")

ToolSvc += DFCommonMuonToolWrapperPreselection
print (DFCommonMuonToolWrapperPreselection)
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperPreselection)
### Decoration of the muon objects with the ID track covariances
#from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonIDCovMatrixDecorator
#DFCommonMuonIDCovMatrixDecorator = DerivationFramework__MuonIDCovMatrixDecorator( name = "DFCommonMuonIDCovMatrixDecorator")
#ToolSvc += DFCommonMuonIDCovMatrixDecorator
#print (DFCommonMuonIDCovMatrixDecorator)
#DFCommonMuonToolWrapperTools.append(DFCommonMuonIDCovMatrixDecorator)

#############
#  Add tools
#############
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel",
                                                                         AugmentationTools = DFCommonMuonToolWrapperTools
                                                                        )

from IsolationAlgs.IsoUpdatedTrackCones import GetUpdatedIsoTrackCones
if not hasattr(DerivationFrameworkJob,"IsolationBuilderTight1000"):
    DerivationFrameworkJob += GetUpdatedIsoTrackCones()