# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# MuonsCommon.py 
# Schedules all tools needed for muon object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# MCP GROUP TOOLS 
#====================================================================
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
DFCommonMuonsSelector = CP__MuonSelectionTool(name = "DFCommonMuonsSelector")
ToolSvc += DFCommonMuonsSelector
print DFCommonMuonsSelector

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
DFCommonMuonToolWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapper",
                                                                        AsgSelectionTool = DFCommonMuonsSelector,
                                                                        CutType = "IDHits",
                                                                        StoreGateEntryName = "DFCommonGoodMuon",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapper
print DFCommonMuonToolWrapper



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel",
                                                                         AugmentationTools = [DFCommonMuonToolWrapper]
                                                                        )
