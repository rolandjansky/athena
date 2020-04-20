# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# updated version of MuonsCommon to extend eta range to 2.8 in muon selection

from __future__ import print_function

#********************************************************************
# MuonsCommon.py
# Schedules all tools needed for muon object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from AthenaCommon.AppMgr import ToolSvc

#====================================================================
# MCP GROUP TOOLS
#====================================================================
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
EXOTDFCommonMuonsSelector = CP__MuonSelectionTool(name = "EXOTDFCommonMuonsSelector")
EXOTDFCommonMuonsSelector.MaxEta = 2.8
ToolSvc += EXOTDFCommonMuonsSelector
print (EXOTDFCommonMuonsSelector)

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
EXOTDFCommonMuonToolWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "EXOTDFCommonMuonToolWrapper",
                                                                        AsgSelectionTool = EXOTDFCommonMuonsSelector,
                                                                        CutType = "IDHits",
                                                                        StoreGateEntryName = "EXOTDFCommonGoodMuon",
                                                                        ContainerName = "Muons")
ToolSvc += EXOTDFCommonMuonToolWrapper
print (EXOTDFCommonMuonToolWrapper)



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from AthenaCommon import CfgMgr 
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("EXOTDFCommonMuonsKernel",
                                                                         AugmentationTools = [EXOTDFCommonMuonToolWrapper]
                                                                        )

