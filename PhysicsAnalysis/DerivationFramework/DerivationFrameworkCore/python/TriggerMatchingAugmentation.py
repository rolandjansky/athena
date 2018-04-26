# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

def applyTriggerMatching(ElectronTriggers=[],MuonTriggers=[],PhotonTriggers=[]):
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__TriggerMatchingAugmentation
    from AthenaCommon.AppMgr import ToolSvc
    DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
        name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
        DecorationPrefix = "DFCommonTrigMatch_",
        ElectronContainerName = "Electrons",
        MuonContainerName = "Muons",
        PhotonContainerName = "Photons",
        SingleTriggerList = ElectronTriggers + MuonTriggers + PhotonTriggers
        )
    ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
    ElTrigVars = []
    MuTrigVars = []
    PhTrigVars = []
    if ElectronTriggers != []: ElTrigVars.append("Electrons."+".".join(["DFCommonTrigMatch_"+t for t in  ElectronTriggers]))
    if MuonTriggers != []: MuTrigVars.append("Muons."+".".join(["DFCommonTrigMatch_"+t for t in  MuonTriggers]))
    if PhotonTriggers != []: PhTrigVars.append("Photons."+".".join(["DFCommonTrigMatch_"+t for t in  PhotonTriggers]))
    Triggers = {"Electrons": ElTrigVars, "Muons": MuTrigVars, "Photons": PhTrigVars}
    return  Triggers
