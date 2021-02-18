# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
#
# ##########################################################################################
# Electron/photon/muon augmentation
# Use the derivation framework tools directly
# ##########################################################################################

# Cache the container names (used throughout)
jetContainer = primRPVLLDESDM.Tau_containerFlags.jetCollectionName
METContainer= primRPVLLDESDM.Tau_containerFlags.METCollectionName
photonContainer = primRPVLLDESDM.Tau_containerFlags.photonCollectionName
muonContainer = primRPVLLDESDM.Tau_containerFlags.muonCollectionName
electronContainer = primRPVLLDESDM.Tau_containerFlags.electronCollectionName


# ##########################################################################################
# Selection algorithms setup
# ##########################################################################################



#Currently not in use. Copied directly from DV analysis
def TauSelectionString(flags, container):
    """Construct object selection string based on the flags provided (from DVFlags)
    """

    cutList = []
    try:  
        cutList.append('%s.pt > %s'%(container,flags.cutEtMin))
    except:
        pass
    try:
        cutList.append('abs(%s.eta) < %s'%(container,flags.cutEtaMax))
    except:
        pass
    try:   
        cutList.append("DV%s%s"%(container,flags.cutIsEM))
    except:
        pass
    cutString = 'count(' + ' && '.join(cutList) + ') > %i'%(flags.nPassed-1)
    return cutString

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

def TausTriggerSelectionString(flags,RNN=False):
    cutString = " || ".join(flags.triggers)
    if RNN:
        cutString = " || ".join([l for l in flags.triggers if "RNN" in l])
    return cutString

############################################################
## Tools for combining filters
###########################################################
 ## needs package PhysicsAnalysis/DerivationFramework/DerivationFrameworkTools-00-00-20 or later..
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR

############################################################
## Tau filters
###########################################################

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

############################################################
## Single Tau RNN filter
###########################################################

TauSingleTriggerFilter = skimtool( name = "TauSingleTriggerFilter",
                                   expression = TausTriggerSelectionString(primRPVLLDESDM.Taus_SingleTauFilterFlags,RNN=True)
)

ToolSvc+=TauSingleTriggerFilter


Taus_SiFinalFilter = DerivationFramework__FilterCombinationAND( name = "LargeRadSiTaus_FinalFilter",
                                                               FilterList=[TauSingleTriggerFilter],
                                                               #OutputLevel=DEBUG
                                                               )
ToolSvc += Taus_SiFinalFilter

topSequence += kernel( "RPVLL_LargeRTaus_SiFilterKernel",
                       SkimmingTools = [Taus_SiFinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_LargeRTaus_SiFilterKernel"])

############################################################
## Di Tau filter
###########################################################
TauDiTriggerFilter = skimtool( name = "TauDiTriggerFilter",
                               expression = TausTriggerSelectionString(primRPVLLDESDM.Taus_DiTauFilterFlags,RNN=True)
)

ToolSvc+=TauDiTriggerFilter

Taus_DiFinalFilter = DerivationFramework__FilterCombinationAND( name = "LargeRadDiTaus_FinalFilter",
                                                                FilterList=[TauDiTriggerFilter],
                                                                #OutputLevel=DEBUG
)
ToolSvc += Taus_DiFinalFilter

topSequence += kernel( "RPVLL_LargeRTaus_DiFilterKernel",
                       SkimmingTools = [Taus_DiFinalFilter],
)
RPVLLfilterNames.extend(["RPVLL_LargeRTaus_DiFilterKernel"])



############################################################
## Met Tau filter
###########################################################

TauMETTriggerFilter = skimtool( name = "TauMETTriggerFilter",
                                   expression = TausTriggerSelectionString(primRPVLLDESDM.Taus_TauMETFilterFlags,RNN=True)
)

ToolSvc+=TauMETTriggerFilter


Taus_METFinalFilter = DerivationFramework__FilterCombinationAND( name = "LargeRadTaus_METFinalFilter",
                                                               FilterList=[TauMETTriggerFilter],
                                                               #OutputLevel=DEBUG
                                                               )
ToolSvc += Taus_METFinalFilter

topSequence += kernel( "RPVLL_LargeRTaus_METFilterKernel",
                       SkimmingTools = [Taus_METFinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_LargeRTaus_METFilterKernel"])

