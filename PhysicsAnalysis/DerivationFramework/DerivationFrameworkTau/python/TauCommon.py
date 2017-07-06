# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# TauCommon.py
# Schedules all tools needed for tau object selection and writes
# results into SG. These may then be accessed along the train.
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# MAKE TAU ENUMS AVAILABLE
#====================================================================
#from ROOT import xAOD__TauJetParameters__IsTauFlag

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauSelectionTool
DFCommonTauWrapperTools = []

# Loose
DFCommonTausSelectorLoose = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorLoose")
DFCommonTausSelectorLoose.JetIDWP = 2
DFCommonTausSelectorLoose.SelectionCuts = 1<<6
#TauAnalysisTools::CutJetIDWP should be used but issue with the dictionnary
ToolSvc += DFCommonTausSelectorLoose
DFCommonTausLooseWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausLooseWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorLoose,
                                                                         StoreGateEntryName   = "DFCommonTausLoose",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausLooseWrapper
print DFCommonTausLooseWrapper
DFCommonTauWrapperTools.append(DFCommonTausLooseWrapper)

# Medium
DFCommonTausSelectorMedium = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorMedium")
DFCommonTausSelectorMedium.JetIDWP = 3
DFCommonTausSelectorMedium.SelectionCuts = 1<<6
ToolSvc += DFCommonTausSelectorMedium
DFCommonTausMediumWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausMediumWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorMedium,
                                                                         StoreGateEntryName   = "DFCommonTausMedium",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausMediumWrapper
print DFCommonTausMediumWrapper
DFCommonTauWrapperTools.append(DFCommonTausMediumWrapper)

# Tight
DFCommonTausSelectorTight = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorTight")
DFCommonTausSelectorTight.JetIDWP = 4
DFCommonTausSelectorTight.SelectionCuts = 1<<6
ToolSvc += DFCommonTausSelectorTight
DFCommonTausTightWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausTightWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorTight,
                                                                         StoreGateEntryName   = "DFCommonTausTight",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausTightWrapper
print DFCommonTausTightWrapper
DFCommonTauWrapperTools.append(DFCommonTausTightWrapper)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TauCommonKernel",
                                                                         AugmentationTools = DFCommonTauWrapperTools)
