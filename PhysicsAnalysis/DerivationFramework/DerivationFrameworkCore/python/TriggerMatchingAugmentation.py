# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# Authors: Johannes Mellenthin, Teng Jian Khoo

# A python helper to set up trigger matching decorations on leptons and photons
# Returns the tool that applies the augmentation, and the new branches to add to ExtraVariables
def applyTriggerMatching(ToolNamePrefix,ElectronTriggers=[],MuonTriggers=[],PhotonTriggers=[]):

    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__TriggerMatchingAugmentation
    from AthenaCommon.AppMgr import ToolSvc

    # Create an instance of the matching tool
    # Use defaults except for the name (need different instances of the tool for different derivations
    # and the list of triggers.
    # For now we only fill the single trigger list, but this can easily be expanded to
    # cope with the dilepton trigger options.
    AugmentationTool = DerivationFramework__TriggerMatchingAugmentation(
        name = ToolNamePrefix+"_TriggerMatchingAugmentation",
        SingleTriggerList = ElectronTriggers + MuonTriggers + PhotonTriggers
        )
    ToolSvc += AugmentationTool

    # Define list of variables to be added to ExtraVariables
    ElTrigVars = ""
    MuTrigVars = ""
    PhTrigVars = ""
    if ElectronTriggers != []:
        ElTrigVars = ".".join(["DFCommonTrigMatch_"+t for t in  ElectronTriggers])
    if MuonTriggers != []:
        MuTrigVars = ".".join(["DFCommonTrigMatch_"+t for t in  MuonTriggers])
    if PhotonTriggers != []:
        PhTrigVars = ".".join(["DFCommonTrigMatch_"+t for t in  PhotonTriggers])

    TrigVars = {"Electrons": ElTrigVars, "Muons": MuTrigVars, "Photons": PhTrigVars}
    return AugmentationTool, TrigVars
