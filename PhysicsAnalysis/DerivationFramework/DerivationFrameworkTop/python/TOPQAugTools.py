# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used for TOPQ augmentation
# Call with:
#     import DerivationFrameworkTop.TOPQCommonTruthTools
#     augmentationTools = DerivationFrameworkTop.TOPQCommonSelection.setup('TOPQ', ToolSvc)
#====================================================================

#================================
# IMPORTS
#================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

def setup_ExKtbb(ToolSvc):

    augmentationTools=[]

    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__ExKtbbAugmentation
    TOPQExKtbbAugmentation = DerivationFramework__ExKtbbAugmentation(name = "TOPQExKtbbAugmentation")
    TOPQExKtbbAugmentation.isMC = (globalflags.DataSource()=='geant4')
    ToolSvc += TOPQExKtbbAugmentation
    augmentationTools.append(TOPQExKtbbAugmentation)
    #=============
    # RETURN TOOLS
    #=============
    return augmentationTools

def setup_SoftSV(ToolSvc):

    augmentationTools=[]

    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__SoftSVAugmentation
    TOPQSoftSVAugmentation = DerivationFramework__SoftSVAugmentation(name = "TOPQSoftSVAugmentation")
    TOPQSoftSVAugmentation.isMC = (globalflags.DataSource()=='geant4')
    ToolSvc += TOPQSoftSVAugmentation
    augmentationTools.append(TOPQSoftSVAugmentation)

    # return tools
    return augmentationTools

#==============================================================================
# SETUP TRUTH KERNEL
#==============================================================================
augmentationTools_ExKtbb = setup_ExKtbb(ToolSvc)
TOPQExKtCommonTruthKernel = CfgMgr.DerivationFramework__CommonAugmentation("TOPQExKtCommonTruthKernel", AugmentationTools = augmentationTools_ExKtbb)

augmentationTools_SoftSV = setup_SoftSV(ToolSvc)
TOPQSoftSVCommonAugmentation = CfgMgr.DerivationFramework__CommonAugmentation("TOPQSoftSVCommonAugmentation", AugmentationTools = augmentationTools_SoftSV)
