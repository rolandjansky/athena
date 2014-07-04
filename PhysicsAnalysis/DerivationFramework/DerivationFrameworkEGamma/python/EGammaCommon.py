# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# EGammaCommon.py 
# Schedules all tools needed for e-gamma object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# ELECTRON 4-MOMENTA CORRECTION
#====================================================================
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__ElectronsDirectionTool
DFCommonElectronsDirection = DerivationFramework__ElectronsDirectionTool(name = "DFCommonElectronsDirection",
                                                                        EtaSGEntry = "DFCommonElectrons_eta",
                                                                        PtSGEntry = "DFCommonElectrons_pt",
                                                                        nPrecisionHitsSGEntry = "DFCommonElectronsnPrecisionHits"
                                                                    )
ToolSvc += DFCommonElectronsDirection

#====================================================================
# ELECTRON SELECTION (loose LLH)
#====================================================================
import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
#from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import electronLHmenu
from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
LhoodSelectorLoose = ConfiguredAsgElectronLikelihoodTool("LhoodSelectorLoose",
                                                         LikeEnum.Loose
                                                         )
LhoodSelectorLoose.primaryVertexContainer = "PrimaryVertices"
ToolSvc += LhoodSelectorLoose

#====================================================================
# ELECTRON SELECTION (multi lepton)
#====================================================================
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronMultiLeptonSelector
MLSelector = AsgElectronMultiLeptonSelector("MLSelector")
ToolSvc += MLSelector

#=======================================
# EGAMMA PASS SELECTION WRAPPERS
#=======================================
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGammaPassSelectionWrapper
DFCommonElectronsSelection = DerivationFramework__EGammaPassSelectionWrapper( name = "DFCommonElectronsSelection",
                                                               		 SelectionVariables = ["Loose","Medium","Tight"],
									 CollectionName = "ElectronCollection", 
									 SGPrefix = "DFCommonElectrons"
                                                             	       )
ToolSvc += DFCommonElectronsSelection
print DFCommonElectronsSelection

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
ElectronPassLHLoose = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassLHLoose",
                                                                    AsgSelectionTool = LhoodSelectorLoose,
                                                                    CutType = "",
                                                                    StoreGateEntryName = "DFCommonElectronsLHLoose",
                                                                    ContainerName = "ElectronCollection")
ToolSvc += ElectronPassLHLoose
print ElectronPassLHLoose

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
ElectronPassML = DerivationFramework__AsgSelectionToolWrapper( name = "ElectronPassML",
                                                                    AsgSelectionTool = MLSelector,
                                                                    CutType = "",
                                                                    StoreGateEntryName = "DFCommonElectronsML",
                                                                    ContainerName = "ElectronCollection")
ToolSvc += ElectronPassML
print ElectronPassML

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("EGammaCommonKernel",
                                                                         AugmentationTools = [DFCommonElectronsDirection,DFCommonElectronsSelection,ElectronPassLHLoose,ElectronPassML]
                                                                         )
