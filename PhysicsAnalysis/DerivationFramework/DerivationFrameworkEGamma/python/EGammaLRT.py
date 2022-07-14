# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# ********************************************************************
# EGammaLRT.py
# Schedules additional tools for LRT electron object selection and writes
# results into SG. These may then be accessed along the train
# ********************************************************************

def makeLRTEGammaDF():

    from DerivationFrameworkCore.DerivationFrameworkMaster import (
        DerivationFrameworkJob, DerivationFrameworkSimBarcodeOffset)
    from AthenaCommon import CfgMgr
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AppMgr import ToolSvc

    # TODO: EGammaVariableCorrection.EGammaVariableCorrectionConf seems to be hardcoded to Electrons container?  

    # ====================================================================
    # Displaced ELECTRON LH SELECTORS
    # see Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py
    # on how to configure the selectors
    # ====================================================================
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import (
        AsgElectronLikelihoodTool)

    ElectronLHSelectorVeryLooseNoPix = AsgElectronLikelihoodTool(
        "ElectronLHSelectorVeryLooseNoPix",
        ConfigFile = "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodVeryLooseTriggerConfig_NoPix.conf")
    ElectronLHSelectorVeryLooseNoPix.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorVeryLooseNoPix

    ElectronLHSelectorLooseNoPix = AsgElectronLikelihoodTool(
        "ElectronLHSelectorLooseNoPix",
        ConfigFile = "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodLooseTriggerConfig_NoPix.conf")
    ElectronLHSelectorLooseNoPix.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorLooseNoPix

    ElectronLHSelectorMediumNoPix = AsgElectronLikelihoodTool(
        "ElectronLHSelectorMediumNoPix",
        ConfigFile = "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodMediumTriggerConfig_NoPix.conf")
    ElectronLHSelectorMediumNoPix.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorMediumNoPix

    ElectronLHSelectorTightNoPix = AsgElectronLikelihoodTool(
        "ElectronLHSelectorTightNoPix",
        ConfigFile = "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodTightTriggerConfig_NoPix.conf")
    ElectronLHSelectorTightNoPix.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorTightNoPix


    # ====================================================================
    # ELECTRON CHARGE SELECTION
    # ====================================================================
    if not hasattr(ToolSvc, "ElectronChargeIDSelectorLoose"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import (
            AsgElectronChargeIDSelectorTool)
        ElectronChargeIDSelector = AsgElectronChargeIDSelectorTool(
            "ElectronChargeIDSelectorLoose")
        ElectronChargeIDSelector.primaryVertexContainer = "PrimaryVertices"
        ElectronChargeIDSelector.TrainingFile = "ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root"
        ToolSvc += ElectronChargeIDSelector


    # ====================================================================
    # AUGMENTATION TOOLS
    # ====================================================================
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGElectronLikelihoodToolWrapper)

    # Decorate lrt electrons with the output of LH veryloose no pix
    ElectronPassLHVeryLooseNoPix = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHVeryLooseNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorVeryLooseNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHVeryLooseNoPix",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHVeryLooseNoPix

    ElectronPassLHVeryLooseNoPixLRT = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="LRTElectronPassLHVeryLooseNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorVeryLooseNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHVeryLooseNoPix",
        ContainerName="LRTElectrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHVeryLooseNoPixLRT

    # Decorate lrt electrons with the output of LH loose no pix
    ElectronPassLHLooseNoPix = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHLooseNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorLooseNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHLooseNoPix",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHLooseNoPix

    ElectronPassLHLooseNoPixLRT = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="LRTElectronPassLHLooseNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorLooseNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHLooseNoPix",
        ContainerName="LRTElectrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHLooseNoPixLRT


    # Decorate lrt electrons with the output of LH medium no pix
    ElectronPassLHMediumNoPix = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHMediumNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorMediumNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHMediumNoPix",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHMediumNoPix

    ElectronPassLHMediumNoPixLRT = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="LRTElectronPassLHMediumNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorMediumNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHMediumNoPix",
        ContainerName="LRTElectrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHMediumNoPixLRT

    # Decorate lrt electrons with the output of LH tight no pix
    ElectronPassLHTightNoPix = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHTightNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorTightNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHTightNoPix",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHTightNoPix

    ElectronPassLHTightNoPixLRT = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="LRTElectronPassLHTightNoPix",
        EGammaElectronLikelihoodTool=ElectronLHSelectorTightNoPix,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHTightNoPix",
        ContainerName="LRTElectrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHTightNoPixLRT


    # Decorate lrt electrons with the output of ECIDS
    LRTElectronPassECIDS = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="LRTElectronPassECIDS",
        EGammaElectronLikelihoodTool=ToolSvc.ElectronChargeIDSelectorLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsECIDS",
        ContainerName="LRTElectrons",
        StoreTResult=True)
    ToolSvc += LRTElectronPassECIDS


    # decorate central lrt electrons with a flag to tell the the
    # candidates are affected by the crack bug in mc16a and data 2015+2016
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGCrackVetoCleaningTool as DF_EGCVCT)
    LRTElectronPassCrackVeto = DF_EGCVCT(
        name="LRTElectronPassCrackVeto",
        StoreGateEntryName="DFCommonCrackVetoCleaning",
        ContainerName="LRTElectrons")
    ToolSvc += [LRTElectronPassCrackVeto]

    # decorate some lrt electrons with an additional ambiguity flag
    # against internal and early material conversion
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGElectronAmbiguityTool as DF_EGEAT)
    LRTElectronAmbiguity = DF_EGEAT(
        name="LRTElectronAdditionnalAmbiguity",
        isMC=(globalflags.DataSource() != 'data'),
        idCut="DFCommonElectronsLHLooseNoPix",
        ContainerName="LRTElectrons")
    ToolSvc += LRTElectronAmbiguity

    # list of all the decorators so far
    LRTEGAugmentationTools = [ElectronPassLHVeryLooseNoPix,
                              ElectronPassLHVeryLooseNoPixLRT,
                              ElectronPassLHLooseNoPix,
                              ElectronPassLHLooseNoPixLRT,
                              ElectronPassLHMediumNoPix,
                              ElectronPassLHMediumNoPixLRT,
                              ElectronPassLHTightNoPix,
                              ElectronPassLHTightNoPixLRT,
                              LRTElectronPassECIDS,
                              LRTElectronPassCrackVeto,
                              LRTElectronAmbiguity]

    # ==================================================
    # Truth Related tools
    from RecExConfig.RecFlags import rec
    if rec.doTruth():
        # Decorate Electron with bkg electron type/origin
        from MCTruthClassifier.MCTruthClassifierBase import (
            MCTruthClassifier as BkgElectronMCTruthClassifier)
        BkgElectronMCTruthClassifier.barcodeG4Shift = (
            DerivationFrameworkSimBarcodeOffset+1)
        BkgElectronMCTruthClassifier.ParticleCaloExtensionTool = ""

        from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
            DerivationFramework__BkgElectronClassification) 
        BkgLRTElectronClassificationTool = DerivationFramework__BkgElectronClassification(
            name="BkgLRTElectronClassificationTool",
            MCTruthClassifierTool=BkgElectronMCTruthClassifier,
            barcodeCut=DerivationFrameworkSimBarcodeOffset,
            ElectronContainerName="LRTElectrons")
        ToolSvc += BkgLRTElectronClassificationTool
        LRTEGAugmentationTools.append(BkgLRTElectronClassificationTool)

    # =======================================
    # CREATE THE DERIVATION KERNEL ALGORITHM
    # =======================================

    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation(
        "LRTEGammaCommonKernel",
        AugmentationTools=LRTEGAugmentationTools
    )

    # =======================================
    # ADD TOOLS
    # =======================================
    import IsolationAlgs.IsoUpdatedTrackCones as isoCones
    if not hasattr(DerivationFrameworkJob, "IsolationBuilderNonprompt_All_MaxWeight1000LRT"):
        DerivationFrameworkJob += isoCones.GetUpdatedIsoTrackCones(postfix="LRT")