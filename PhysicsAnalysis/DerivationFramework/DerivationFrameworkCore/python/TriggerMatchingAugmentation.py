from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__TriggerMatchingAugmentation
def TriggerMatchingAugmentation(Electrons,Muons,Photons,ToolSvc):
    if Electrons != "" and Muons != "" and Photons != "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            ElectronContainerName = "Electrons",
            MuonContainerName = "Muons",
            PhotonContainerName = "Photons",
            SingleTriggerList = Electrons + Muons + Photons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        ElTrigVars ="Electrons."+".".join(["DFCommonTrigMatch_"+t for t in  Electrons])
        MuTrigVars ="Muons."+".".join(["DFCommonTrigMatch_"+t for t in  Muons])
        PhTrigVars ="Photons."+".".join(["DFCommonTrigMatch_"+t for t in  Photons])
        return  [ElTrigVars, MuTrigVars, PhTrigVars]
    elif Electrons != "" and Muons != "" and Photons == "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            ElectronContainerName = "Electrons",
            MuonContainerName = "Muons",
            SingleTriggerList = Electrons + Muons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        ElTrigVars ="Electrons."+".".join(["DFCommonTrigMatch_"+t for t in  Electrons])
        MuTrigVars ="Muons."+".".join(["DFCommonTrigMatch_"+t for t in  Muons])
        return [ElTrigVars, MuTrigVars, ""]
    elif Electrons != "" and Muons == "" and Photons != "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            ElectronContainerName = "Electrons",
            PhotonContainerName = "Photons",
            SingleTriggerList = Electrons + Photons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        ElTrigVars ="Electrons."+".".join(["DFCommonTrigMatch_"+t for t in  Electrons])
        PhTrigVars ="Photons."+".".join(["DFCommonTrigMatch_"+t for t in  Photons])
        return [ElTrigVars, "", PhTrigVars]
    elif Electrons != "" and Muons != "" and Photons == "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            MuonContainerName = "Muons",
            PhotonContainerName = "Photons",
            SingleTriggerList = Muons + Photons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        MuTrigVars ="Muons."+".".join(["DFCommonTrigMatch_"+t for t in  Muons])
        PhTrigVars ="Photons."+".".join(["DFCommonTrigMatch_"+t for t in  Photons])
        return ["", MuTrigVars, PhTrigVars]
    elif Electrons != "" and Muons == "" and Photons == "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            ElectronContainerName = "Electrons",
            SingleTriggerList = Electrons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        ElTrigVars ="Electrons."+".".join(["DFCommonTrigMatch_"+t for t in  Electrons])
        return [ElTrigVars, "", ""]
    elif Electrons == "" and Muons != "" and Photons == "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            MuonContainerName = "Muons",
            SingleTriggerList = Muons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        MuTrigVars ="Muons."+".".join(["DFCommonTrigMatch_"+t for t in  Muons])
        return ["", MuTrigVars, ""]
    elif Electrons == "" and Muons == "" and Photons != "":
        DFCommonTrigMatch_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
            name = "DFCommonTrigMatch_TriggerMatchingAugmentation",
            DecorationPrefix = "DFCommonTrigMatch_",
            PhotonContainerName = "Photons",
            SingleTriggerList = Photons
            )
        ToolSvc += DFCommonTrigMatch_TriggerMatchingAugmentation
        PhTrigVars ="Photons."+".".join(["DFCommonTrigMatch_"+t for t in  Photons])
        return ["", "", PhTrigVars]
    elif Electrons == "" and Muons == "" and Photons == "":
        print "ERROR: No TriggerMatchingAugmentation input!"
