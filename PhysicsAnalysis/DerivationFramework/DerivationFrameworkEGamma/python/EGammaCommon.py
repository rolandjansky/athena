# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# ********************************************************************
# EGammaCommon.py
# Schedules all tools needed for e-gamma object selection and writes
# results into SG. These may then be accessed along the train
# ********************************************************************

def makeEGammaDFCommon():

    from DerivationFrameworkCore.DerivationFrameworkMaster import (
        DerivationFrameworkJob, DerivationFrameworkSimBarcodeOffset)
    from AthenaCommon import CfgMgr
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AppMgr import ToolSvc, ServiceMgr as svcMgr

    # ====================================================================
    # PHOTON ETA (=ETA2), ET (=E/COSH(ETA2))
    # ====================================================================
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__PhotonsDirectionTool)
    DFCommonPhotonsDirection = DerivationFramework__PhotonsDirectionTool(
        name="DFCommonPhotonsDirection",
        EtaSGEntry="DFCommonPhotons_eta",
        PhiSGEntry="DFCommonPhotons_phi",
        EtSGEntry="DFCommonPhotons_et"
    )
    ToolSvc += DFCommonPhotonsDirection

    # ====================================================================
    # SHOWER SHAPE CORRECTIONS IN MC
    # (TUNE23: FUDGE FACTORS RUN2 FULL DATA, derived with rel 21.2)
    # ====================================================================

    from PyUtils import AthFile
    # opens the first file from the InputCollections list
    af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
    # this is a dict of dicts, take a look at what's available!
    # Below are some examples:
    af.fileinfos

    isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
    isFullSim = False
    if isMC:
        simulationFlavour = af.fileinfos['metadata']['/Simulation/Parameters']['SimulationFlavour']
        isFullSim = simulationFlavour in ('default', 'MC12G4', 'FullG4')

    print("EGammaCommon: isMC = ", isMC)
    if isMC:
        print("EGammaCommon: isFullSim = ", isFullSim)

    if isFullSim:
        from EGammaVariableCorrection.EGammaVariableCorrectionConf import (
            ElectronPhotonVariableCorrectionTool)
        EGVariableCorrectionTool = ElectronPhotonVariableCorrectionTool(
            name='EGVariableCorrectionTool',
            ConfigFile='EGammaVariableCorrection/TUNE23/ElPhVariableNominalCorrection.conf')
        ToolSvc += EGVariableCorrectionTool
        print(EGVariableCorrectionTool)

    # ====================================================================
    # ELECTRON LH SELECTORS
    # see Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py
    # on how to configure the selectors
    # ====================================================================
    from ROOT import LikeEnum

    from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import (
        ConfiguredAsgElectronLikelihoodTool)

    # Very Loose
    ElectronLHSelectorVeryLoose = ConfiguredAsgElectronLikelihoodTool(
        "ElectronLHSelectorVeryLoose", LikeEnum.VeryLoose)
    ElectronLHSelectorVeryLoose.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorVeryLoose

    # Loose
    ElectronLHSelectorLoose = ConfiguredAsgElectronLikelihoodTool(
        "ElectronLHSelectorLoose", LikeEnum.Loose)
    ElectronLHSelectorLoose.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorLoose

    # Medium
    ElectronLHSelectorMedium = ConfiguredAsgElectronLikelihoodTool(
        "ElectronLHSelectorMedium", LikeEnum.Medium)
    ElectronLHSelectorMedium.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorMedium

    # Tight
    ElectronLHSelectorTight = ConfiguredAsgElectronLikelihoodTool(
        "ElectronLHSelectorTight", LikeEnum.Tight)
    ElectronLHSelectorTight.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorTight

    # LooseBL
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import (
        AsgElectronLikelihoodTool)
    ElectronLHSelectorLooseBL = AsgElectronLikelihoodTool(
        "ElectronLHSelectorLooseBL", WorkingPoint="LooseBLLHElectron")
    ElectronLHSelectorLooseBL.primaryVertexContainer = "PrimaryVertices"
    ToolSvc += ElectronLHSelectorLooseBL

    # ====================================================================
    # ELECTRON CHARGE SELECTION
    # ====================================================================
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import (
        AsgElectronChargeIDSelectorTool)
    ElectronChargeIDSelector = AsgElectronChargeIDSelectorTool(
        "ElectronChargeIDSelectorLoose")
    ElectronChargeIDSelector.primaryVertexContainer = "PrimaryVertices"
    ElectronChargeIDSelector.TrainingFile = "ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root"
    ToolSvc += ElectronChargeIDSelector

    # ====================================================================
    # FWD ELECTRON LH SELECTORS
    # ====================================================================
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import (
        AsgForwardElectronLikelihoodTool)

    ForwardElectronLHSelectorLoose = AsgForwardElectronLikelihoodTool(
        "ForwardElectronLHSelectorLoose",
        WorkingPoint="LooseLHForwardElectron")
    ToolSvc += ForwardElectronLHSelectorLoose

    ForwardElectronLHSelectorMedium = AsgForwardElectronLikelihoodTool(
        "ForwardElectronLHSelectorMedium",
        WorkingPoint="MediumLHForwardElectron")
    ToolSvc += ForwardElectronLHSelectorMedium

    ForwardElectronLHSelectorTight = AsgForwardElectronLikelihoodTool(
        "ForwardElectronLHSelectorTight",
        WorkingPoint="TightLHForwardElectron")
    ToolSvc += ForwardElectronLHSelectorTight

    # ====================================================================
    # PHOTON SELECTION (loose and tight cut-based)
    # ====================================================================
    from ROOT import egammaPID

    from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import (
        ConfiguredAsgPhotonIsEMSelector)
    from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import (
        photonPIDmenu)

    # Loose
    PhotonIsEMSelectorLoose = ConfiguredAsgPhotonIsEMSelector(
        "PhotonIsEMSelectorLoose", egammaPID.PhotonIDLoose)
    ToolSvc += PhotonIsEMSelectorLoose

    # Tight (default == pt-dependent)
    PhotonIsEMSelectorTight = ConfiguredAsgPhotonIsEMSelector(
        "PhotonIsEMSelectorTight", egammaPID.PhotonIDTight)
    ToolSvc += PhotonIsEMSelectorTight

    # Tight (pt-inclusive)
    # To be removed when pt-dependent menu above is supported with scale factors
    PhotonIsEMSelectorTightPtIncl = ConfiguredAsgPhotonIsEMSelector(
        "PhotonIsEMSelectorTightPtIncl", egammaPID.PhotonIDTight,
        menu=photonPIDmenu.menuPtInclJan2018)
    ToolSvc += PhotonIsEMSelectorTightPtIncl

    # ====================================================================
    # RECTANGULAR CLUSTER TOOLS
    # ====================================================================

    from CaloClusterCorrection.CaloClusterCorrectionConf import (
        CaloFillRectangularCluster)

    EGAMCOM_caloFillRect55 = CaloFillRectangularCluster(
        name="EGAMCOMCaloFillRectangularCluster55",
        cells_name="AllCalo",
        eta_size=5,
        phi_size=5,
        fill_cluster=True)
    ToolSvc += EGAMCOM_caloFillRect55

    EGAMCOM_caloFillRect35 = CaloFillRectangularCluster(
        name="EGAMCOMCaloFillRectangularCluster35",
        cells_name="AllCalo",
        eta_size=3,
        phi_size=5,
        fill_cluster=True)
    ToolSvc += EGAMCOM_caloFillRect35

    EGAMCOM_caloFillRect37 = CaloFillRectangularCluster(
        name="EGAMCOMCaloFillRectangularCluster37",
        cells_name="AllCalo",
        eta_size=3,
        phi_size=7,
        fill_cluster=True)
    ToolSvc += EGAMCOM_caloFillRect37

    EGAMCOM_caloFillRect711 = CaloFillRectangularCluster(
        name="EGAMCOMCaloFillRectangularCluster711",
        cells_name="AllCalo",
        eta_size=7,
        phi_size=11,
        fill_cluster=True)
    ToolSvc += EGAMCOM_caloFillRect711

    # ====================================================================
    # AUGMENTATION TOOLS
    # ====================================================================
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGSelectionToolWrapper)
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGElectronLikelihoodToolWrapper)

    # decorate electrons with the output of LH very loose
    ElectronPassLHVeryLoose = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHVeryLoose",
        EGammaElectronLikelihoodTool=ElectronLHSelectorVeryLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHVeryLoose",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHVeryLoose
    print(ElectronPassLHVeryLoose)

    # decorate electrons with the output of LH loose
    ElectronPassLHLoose = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHLoose",
        EGammaElectronLikelihoodTool=ElectronLHSelectorLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHLoose",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHLoose
    print(ElectronPassLHLoose)

    # decorate electrons with the output of LH loose+BL
    ElectronPassLHLooseBL = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHLooseBL",
        EGammaElectronLikelihoodTool=ElectronLHSelectorLooseBL,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHLooseBL",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHLooseBL
    print(ElectronPassLHLooseBL)

    # decorate electrons with the output of LH medium
    ElectronPassLHMedium = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHMedium",
        EGammaElectronLikelihoodTool=ElectronLHSelectorMedium,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHMedium",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHMedium
    print(ElectronPassLHMedium)

    # decorate electrons with the output of LH tight
    ElectronPassLHTight = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassLHTight",
        EGammaElectronLikelihoodTool=ElectronLHSelectorTight,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHTight",
        ContainerName="Electrons",
        StoreTResult=False)
    ToolSvc += ElectronPassLHTight
    print(ElectronPassLHTight)

    # decorate electrons with the output of ECIDS
    ElectronPassECIDS = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ElectronPassECIDS",
        EGammaElectronLikelihoodTool=ElectronChargeIDSelector,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsECIDS",
        ContainerName="Electrons",
        StoreTResult=True)
    ToolSvc += ElectronPassECIDS
    print(ElectronPassECIDS)

    # decorate forward electrons with the output of LH loose
    ForwardElectronPassLHLoose = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ForwardElectronPassLHLoose",
        EGammaElectronLikelihoodTool=ForwardElectronLHSelectorLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonForwardElectronsLHLoose",
        ContainerName="ForwardElectrons")
    ToolSvc += ForwardElectronPassLHLoose
    print(ForwardElectronPassLHLoose)

    # decorate forward electrons with the output of LH medium
    ForwardElectronPassLHMedium = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ForwardElectronPassLHMedium",
        EGammaElectronLikelihoodTool=ForwardElectronLHSelectorMedium,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonForwardElectronsLHMedium",
        ContainerName="ForwardElectrons")
    ToolSvc += ForwardElectronPassLHMedium
    print(ForwardElectronPassLHMedium)

    # decorate forward electrons with the output of LH tight
    ForwardElectronPassLHTight = DerivationFramework__EGElectronLikelihoodToolWrapper(
        name="ForwardElectronPassLHTight",
        EGammaElectronLikelihoodTool=ForwardElectronLHSelectorTight,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonForwardElectronsLHTight",
        ContainerName="ForwardElectrons")
    ToolSvc += ForwardElectronPassLHTight
    print(ForwardElectronPassLHTight)

    # decorate photons with the output of IsEM loose
    # on MC, fudge the shower shapes before computing the ID (but the
    # original shower shapes are not overridden)
    if isFullSim:
        PhotonPassIsEMLoose = DerivationFramework__EGSelectionToolWrapper(
            name="PhotonPassIsEMLoose",
            EGammaSelectionTool=PhotonIsEMSelectorLoose,
            EGammaFudgeMCTool=EGVariableCorrectionTool,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMLoose",
            ContainerName="Photons")
    else:
        PhotonPassIsEMLoose = DerivationFramework__EGSelectionToolWrapper(
            name="PhotonPassIsEMLoose",
            EGammaSelectionTool=PhotonIsEMSelectorLoose,
            EGammaFudgeMCTool=None,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMLoose",
            ContainerName="Photons")
    ToolSvc += PhotonPassIsEMLoose
    print(PhotonPassIsEMLoose)

    # decorate photons with the output of IsEM tight
    # on full-sim MC, fudge the shower shapes before computing the ID
    # (but the original shower shapes are not overridden)
    if isFullSim:
        PhotonPassIsEMTight = DerivationFramework__EGSelectionToolWrapper(
            name="PhotonPassIsEMTight",
            EGammaSelectionTool=PhotonIsEMSelectorTight,
            EGammaFudgeMCTool=EGVariableCorrectionTool,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMTight",
            ContainerName="Photons")
    else:
        PhotonPassIsEMTight = DerivationFramework__EGSelectionToolWrapper(
            name="PhotonPassIsEMTight",
            EGammaSelectionTool=PhotonIsEMSelectorTight,
            EGammaFudgeMCTool=None,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMTight",
            ContainerName="Photons")
    ToolSvc += PhotonPassIsEMTight
    print(PhotonPassIsEMTight)

    # decorate photons with the output of IsEM tight pt-inclusive menu
    # Can be removed once pt-dependent cuts are fully supported.
    # On full-sim MC, fudge the shower shapes before computing the ID
    # (but the original shower shapes are not overridden)
    PhotonPassIsEMTightPtIncl = DerivationFramework__EGSelectionToolWrapper(
        name="PhotonPassIsEMTightPtIncl",
        EGammaSelectionTool=PhotonIsEMSelectorTightPtIncl,
        EGammaFudgeMCTool=(
            EGVariableCorrectionTool if isFullSim else None),
        CutType="",
        StoreGateEntryName="DFCommonPhotonsIsEMTightPtIncl",
        ContainerName="Photons")
    ToolSvc += PhotonPassIsEMTightPtIncl
    print(PhotonPassIsEMTightPtIncl)

    # decorate photons with the photon cleaning flags
    # on MC, fudge the shower shapes before computing the flags
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGPhotonCleaningWrapper)
    if isFullSim:
        PhotonPassCleaning = DerivationFramework__EGPhotonCleaningWrapper(
            name="PhotonPassCleaning",
            EGammaFudgeMCTool=EGVariableCorrectionTool,
            StoreGateEntryName="DFCommonPhotonsCleaning",
            ContainerName="Photons")
    else:
        PhotonPassCleaning = DerivationFramework__EGPhotonCleaningWrapper(
            name="PhotonPassCleaning",
            EGammaFudgeMCTool=None,
            StoreGateEntryName="DFCommonPhotonsCleaning",
            ContainerName="Photons")
    ToolSvc += PhotonPassCleaning
    print(PhotonPassCleaning)

    # decorate central electrons and photons with a flag to tell the the
    # candidates are affected by the crack bug in mc16a and data 2015+2016
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGCrackVetoCleaningTool as DF_EGCVCT)
    PhotonPassCrackVeto = DF_EGCVCT(
        name="PhotonPassCrackVeto",
        StoreGateEntryName="DFCommonCrackVetoCleaning",
        ContainerName="Photons")
    ElectronPassCrackVeto = DF_EGCVCT(
        name="ElectronPassCrackVeto",
        StoreGateEntryName="DFCommonCrackVetoCleaning",
        ContainerName="Electrons")
    ToolSvc += [PhotonPassCrackVeto, ElectronPassCrackVeto]

    # decorate some electrons with an additional ambiguity flag
    # against internal and early material conversion
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import (
        DerivationFramework__EGElectronAmbiguityTool as DF_EGEAT)
    ElectronAmbiguity = DF_EGEAT(
        name="ElectronAdditionnalAmbiguity",
        isMC=(globalflags.DataSource() != 'data'))
    ToolSvc += ElectronAmbiguity

    # list of all the decorators so far
    EGAugmentationTools = [DFCommonPhotonsDirection,
                           ElectronPassLHVeryLoose,
                           ElectronPassLHLoose,
                           ElectronPassLHLooseBL,
                           ElectronPassLHMedium,
                           ElectronPassLHTight,
                           ForwardElectronPassLHLoose,
                           ForwardElectronPassLHMedium,
                           ForwardElectronPassLHTight,
                           ElectronPassECIDS,
                           PhotonPassIsEMLoose,
                           PhotonPassIsEMTight,
                           PhotonPassIsEMTightPtIncl,
                           PhotonPassCleaning,
                           PhotonPassCrackVeto,
                           ElectronPassCrackVeto,
                           ElectronAmbiguity]

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
        BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification(
            name="BkgElectronClassificationTool",
            MCTruthClassifierTool=BkgElectronMCTruthClassifier,
            barcodeCut=DerivationFrameworkSimBarcodeOffset)
        ToolSvc += BkgElectronClassificationTool
        print(BkgElectronClassificationTool)
        EGAugmentationTools.append(BkgElectronClassificationTool)

        # Decorate egammaTruthParticles with truth-particle-level etcone20,30,40
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import (
            DerivationFramework__TruthIsolationTool as dtit)
        TruthEgetIsolationTool = dtit(name="TruthEgetIsolationTool",
                                      isoParticlesKey="egammaTruthParticles",
                                      allParticlesKey="TruthParticles",
                                      particleIDsToCalculate=[-11, 11, 22],
                                      IsolationConeSizes=[0.2, 0.3, 0.4],
                                      excludeIDsFromCone=[-16, -
                                                          14, -13, -12, 12, 13, 14, 16],
                                      IsolationVarNamePrefix='etcone',
                                      ChargedParticlesOnly=False)
        ToolSvc += TruthEgetIsolationTool
        print(TruthEgetIsolationTool)
        EGAugmentationTools.append(TruthEgetIsolationTool)

        # Decorate egammaTruthParticles with truth-particle-level ptcone20,30,40
        TruthEgptIsolationTool = dtit(name="TruthEgptIsolationTool",
                                      isoParticlesKey="egammaTruthParticles",
                                      allParticlesKey="TruthParticles",
                                      particleIDsToCalculate=[-11, 11, 22],
                                      IsolationConeSizes=[0.2, 0.3, 0.4],
                                      IsolationVarNamePrefix='ptcone',
                                      ChargedParticlesOnly=True)
        ToolSvc += TruthEgptIsolationTool
        print(TruthEgptIsolationTool)
        EGAugmentationTools.append(TruthEgptIsolationTool)

        # Compute the truth-particle-level energy density in the central eta region
        from EventShapeTools.EventDensityConfig import (
            configEventDensityTool, EventDensityAthAlg)

        # Schedule PseudoJetTruth
        from JetRecConfig.JetRecConfig import getInputAlgs,getConstitPJGAlg,reOrderAlgs
        from JetRecConfig.StandardJetConstits import stdConstitDic as cst
        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.AllConfigFlags import ConfigFlags

        constit_algs = getInputAlgs(cst.Truth, configFlags=ConfigFlags)
        constit_algs = reOrderAlgs( [a for a in constit_algs if a is not None])

        for a in constit_algs:
            if not hasattr(DerivationFrameworkJob,a.getName()):
                DerivationFrameworkJob += conf2toConfigurable(a)

        constitPJAlg = getConstitPJGAlg(cst.Truth, suffix=None)
        if not hasattr(DerivationFrameworkJob,constitPJAlg.getName()):
            DerivationFrameworkJob += conf2toConfigurable(constitPJAlg)

        tc = configEventDensityTool("EDTruthCentralTool", cst.Truth.label,
                                    0.5,
                                    AbsRapidityMin=0.0,
                                    AbsRapidityMax=1.5,
                                    OutputContainer="TruthIsoCentralEventShape",
                                    OutputLevel=3
                                    )
        ToolSvc += tc

        # Compute the truth-particle-level energy density in the forward eta region
        tf = configEventDensityTool("EDTruthForwardTool", cst.Truth.label,
                                    0.5,
                                    AbsRapidityMin=1.5,
                                    AbsRapidityMax=3.0,
                                    OutputContainer="TruthIsoForwardEventShape",
                                    OutputLevel=3,
                                    )
        ToolSvc += tf

        DerivationFrameworkJob += EventDensityAthAlg("EDTruthCentralAlg", EventDensityTool=tc)
        DerivationFrameworkJob += EventDensityAthAlg("EDTruthForwardAlg", EventDensityTool=tf)

    # =======================================
    # CREATE THE DERIVATION KERNEL ALGORITHM
    # =======================================

    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation(
        "EGammaCommonKernel",
        AugmentationTools=EGAugmentationTools
    )

    # =======================================
    # ADD TOOLS
    # =======================================

    import IsolationAlgs.IsoUpdatedTrackCones as isoCones
    if not hasattr(DerivationFrameworkJob, "IsolationBuilderNonprompt_All_MaxWeight1000"):
        DerivationFrameworkJob += isoCones.GetUpdatedIsoTrackCones()
