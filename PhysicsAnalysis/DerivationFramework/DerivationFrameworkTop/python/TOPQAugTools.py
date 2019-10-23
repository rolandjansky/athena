# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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

def setup(ToolSvc):

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

#==============================================================================
# SETUP TRUTH KERNEL
#==============================================================================
augmentationTools = setup(ToolSvc)
TOPQExKtCommonTruthKernel = CfgMgr.DerivationFramework__CommonAugmentation("TOPQExKtCommonTruthKernel", AugmentationTools = augmentationTools)
