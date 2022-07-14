# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# ********************************************************************
# EGammaCommonConfig.py
# Configures  all tools needed for e-gamma object selection and sets
# up the kernel algorithms so the results can be accessed/written to 
# the DAODs. 
# Component accumulator version.
# ********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def EGammaCommonCfg(ConfigFlags):
    """Main config method for e-gamma decorations"""

    acc = ComponentAccumulator()

    # ====================================================================
    # PHOTON ETA (=ETA2), ET (=E/COSH(ETA2))
    # ====================================================================
    from DerivationFrameworkEGamma.EGammaToolsConfig import PhotonsDirectionToolCfg
    
    DFCommonPhotonsDirection = acc.getPrimaryAndMerge(PhotonsDirectionToolCfg(
        ConfigFlags,
        name="DFCommonPhotonsDirection",
        EtaSGEntry="DFCommonPhotons_eta",
        PhiSGEntry="DFCommonPhotons_phi",
        EtSGEntry="DFCommonPhotons_et")
    )

    # ====================================================================
    # SHOWER SHAPE CORRECTIONS IN MC
    # (TUNE23: FUDGE FACTORS RUN2 FULL DATA, derived with rel 21.2)
    # ====================================================================

    # TODO: following lines my not be needed depending on how we get the sim flavour
    #from PyUtils import AthFile
    # info from the ConfigFlags
    # opens the first file from the InputCollections list
    #af = AthFile.fopen(ConfigFlags.Input)
    # this is a dict of dicts, take a look at what's available!
    # Below are some examples:
    #af.fileinfos

    isMC = ConfigFlags.Input.isMC
    isFullSim = False
    if isMC:
        isFullSim = ConfigFlags.Sim.ISF.Simulator.isFullSim()

    print("EGammaCommon: isMC = ", isMC)
    if isMC:
        print("EGammaCommon: isFullSim = ", isFullSim)

    if isFullSim:
        from EGammaVariableCorrection.EGammaVariableCorrectionConfig import ElectronPhotonVariableCorrectionToolCfg
        EGVariableCorrectionTool = acc.popToolsAndMerge(ElectronPhotonVariableCorrectionToolCfg(
            ConfigFlags,
            name='EGVariableCorrectionTool',
            ConfigFile='EGammaVariableCorrection/TUNE23/ElPhVariableNominalCorrection.conf'))
        acc.addPublicTool(EGVariableCorrectionTool)

    # ====================================================================
    # ELECTRON LH SELECTORS
    # see Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py
    # on how to configure the selectors
    # ====================================================================
    from ROOT import LikeEnum

    from ElectronPhotonSelectorTools.AsgElectronLikelihoodToolsConfig import AsgElectronLikelihoodToolCfg
    from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import electronLHmenu

    lhMenu = electronLHmenu.offlineMC21
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
    if geoFlags.Run() == "RUN2" :
        lhMenu = electronLHmenu.offlineMC20

    # Very Loose
    ElectronLHSelectorVeryLoose = acc.popToolsAndMerge(AsgElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ElectronLHSelectorVeryLoose", 
        quality = LikeEnum.VeryLoose,
        menu=lhMenu)
    )
    ElectronLHSelectorVeryLoose.primaryVertexContainer = "PrimaryVertices"
    acc.addPublicTool(ElectronLHSelectorVeryLoose)

    # Loose
    ElectronLHSelectorLoose = acc.popToolsAndMerge(AsgElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ElectronLHSelectorLoose", 
        quality=LikeEnum.Loose,
        menu=lhMenu)
    )
    ElectronLHSelectorLoose.primaryVertexContainer = "PrimaryVertices"
    acc.addPublicTool(ElectronLHSelectorLoose)

    # LooseBL
    ElectronLHSelectorLooseBL = acc.popToolsAndMerge(AsgElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ElectronLHSelectorLooseBL",
        quality=LikeEnum.LooseBL,
        menu=lhMenu)
    )
    ElectronLHSelectorLooseBL.primaryVertexContainer = "PrimaryVertices"
    acc.addPublicTool(ElectronLHSelectorLooseBL)

    # Medium
    ElectronLHSelectorMedium = acc.popToolsAndMerge(AsgElectronLikelihoodToolCfg(
        ConfigFlags,    
        name="ElectronLHSelectorMedium", 
        quality=LikeEnum.Medium,
        menu=lhMenu)
    )
    ElectronLHSelectorMedium.primaryVertexContainer = "PrimaryVertices"
    acc.addPublicTool(ElectronLHSelectorMedium)

    # Tight
    ElectronLHSelectorTight = acc.popToolsAndMerge(AsgElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ElectronLHSelectorTight", 
        quality=LikeEnum.Tight,
        menu=lhMenu)
    )
    ElectronLHSelectorTight.primaryVertexContainer = "PrimaryVertices"
    acc.addPublicTool(ElectronLHSelectorTight)


    # ====================================================================
    # ELECTRON DNN SELECTORS
    # ====================================================================
    from ElectronPhotonSelectorTools.AsgElectronSelectorToolConfig import AsgElectronSelectorToolCfg 
    # Loose
    ElectronDNNSelectorLoose = acc.popToolsAndMerge(AsgElectronSelectorToolCfg(
        ConfigFlags,
        name="ElectronDNNSelectorLoose", 
        WorkingPoint="LooseDNNElectron"))
    acc.addPublicTool(ElectronDNNSelectorLoose)

    # Medium
    ElectronDNNSelectorMedium = acc.popToolsAndMerge(AsgElectronSelectorToolCfg(
        ConfigFlags,
        name="ElectronDNNSelectorMedium", 
        WorkingPoint="MediumDNNElectron"))
    acc.addPublicTool(ElectronDNNSelectorMedium)

    # Tight
    ElectronDNNSelectorTight = acc.popToolsAndMerge(AsgElectronSelectorToolCfg(
        ConfigFlags,
        name="ElectronDNNSelectorTight",
        WorkingPoint="TightDNNElectron"))
    acc.addPublicTool(ElectronDNNSelectorTight)

    # ====================================================================
    # ELECTRON CHARGE SELECTION
    # ====================================================================
    from ElectronPhotonSelectorTools.AsgElectronChargeIDSelectorToolConfig import AsgElectronChargeIDSelectorToolCfg
    ElectronChargeIDSelector = acc.popToolsAndMerge(AsgElectronChargeIDSelectorToolCfg(
        ConfigFlags,
        name="ElectronChargeIDSelectorLoose"))
    ElectronChargeIDSelector.primaryVertexContainer = "PrimaryVertices"
    ElectronChargeIDSelector.TrainingFile = "ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root"
    acc.addPublicTool(ElectronChargeIDSelector)

    # ====================================================================
    # FWD ELECTRON LH SELECTORS
    # ====================================================================
    from ElectronPhotonSelectorTools.AsgForwardElectronLikelihoodToolConfig import AsgForwardElectronLikelihoodToolCfg

    ForwardElectronLHSelectorLoose = acc.popToolsAndMerge(AsgForwardElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ForwardElectronLHSelectorLoose",
        WorkingPoint="LooseLHForwardElectron"))
    acc.addPublicTool(ForwardElectronLHSelectorLoose)

    ForwardElectronLHSelectorMedium = acc.popToolsAndMerge(AsgForwardElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ForwardElectronLHSelectorMedium",
        WorkingPoint="MediumLHForwardElectron"))
    acc.addPublicTool(ForwardElectronLHSelectorMedium)

    ForwardElectronLHSelectorTight = acc.popToolsAndMerge(AsgForwardElectronLikelihoodToolCfg(
        ConfigFlags,
        name="ForwardElectronLHSelectorTight",
        WorkingPoint="TightLHForwardElectron"))
    acc.addPublicTool(ForwardElectronLHSelectorTight)

    # ====================================================================
    # PHOTON SELECTION (loose and tight cut-based)
    # ====================================================================
    from ROOT import egammaPID

    from ElectronPhotonSelectorTools.AsgPhotonIsEMSelectorsConfig import AsgPhotonIsEMSelectorCfg
    from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import photonPIDmenu

    # Loose
    PhotonIsEMSelectorLoose = acc.popToolsAndMerge(AsgPhotonIsEMSelectorCfg(
        ConfigFlags,
        name="PhotonIsEMSelectorLoose",
        quality=egammaPID.PhotonIDLoose))
    acc.addPublicTool(PhotonIsEMSelectorLoose)

    # Tight (default == pt-dependent)
    PhotonIsEMSelectorTight = acc.popToolsAndMerge(AsgPhotonIsEMSelectorCfg(
        ConfigFlags,
        name="PhotonIsEMSelectorTight", 
        quality=egammaPID.PhotonIDTight))
    acc.addPublicTool(PhotonIsEMSelectorTight)

    # Tight (pt-inclusive)
    # To be removed when pt-dependent menu above is supported with scale factors
    PhotonIsEMSelectorTightPtIncl = acc.popToolsAndMerge(AsgPhotonIsEMSelectorCfg(
        ConfigFlags,
        name="PhotonIsEMSelectorTightPtIncl", 
        quality=egammaPID.PhotonIDTight,
        menu=photonPIDmenu.menuPtInclJan2018))
    acc.addPublicTool(PhotonIsEMSelectorTightPtIncl)

    # ====================================================================
    # RECTANGULAR CLUSTER TOOLS
    # ====================================================================

    from egammaCaloTools.egammaCaloToolsConfig import CaloFillRectangularClusterCfg

    EGAMCOM_caloFillRect55 = acc.popToolsAndMerge(CaloFillRectangularClusterCfg(
        ConfigFlags,
        name="EGAMCOMCaloFillRectangularCluster55",
        cells_name="AllCalo",
        eta_size=5,
        phi_size=5,
        fill_cluster=True))
    acc.addPublicTool(EGAMCOM_caloFillRect55)

    EGAMCOM_caloFillRect35 = acc.popToolsAndMerge(CaloFillRectangularClusterCfg(
        ConfigFlags,
        name="EGAMCOMCaloFillRectangularCluster35",
        cells_name="AllCalo",
        eta_size=3,
        phi_size=5,
        fill_cluster=True))
    acc.addPublicTool(EGAMCOM_caloFillRect35)

    EGAMCOM_caloFillRect37 = acc.popToolsAndMerge(CaloFillRectangularClusterCfg(
        ConfigFlags,
        name="EGAMCOMCaloFillRectangularCluster37",
        cells_name="AllCalo",
        eta_size=3,
        phi_size=7,
        fill_cluster=True))
    acc.addPublicTool(EGAMCOM_caloFillRect37)

    EGAMCOM_caloFillRect711 = acc.popToolsAndMerge(CaloFillRectangularClusterCfg(
        ConfigFlags,
        name="EGAMCOMCaloFillRectangularCluster711",
        cells_name="AllCalo",
        eta_size=7,
        phi_size=11,
        fill_cluster=True))
    acc.addPublicTool(EGAMCOM_caloFillRect711)

    # ====================================================================
    # AUGMENTATION TOOLS
    # ====================================================================
    from DerivationFrameworkEGamma.EGammaToolsConfig import EGSelectionToolWrapperCfg
    from DerivationFrameworkEGamma.EGammaToolsConfig import EGElectronLikelihoodToolWrapperCfg

    # decorate electrons with the output of LH very loose
    ElectronPassLHVeryLoose = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ElectronPassLHVeryLoose",
        EGammaElectronLikelihoodTool=ElectronLHSelectorVeryLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHVeryLoose",
        ContainerName="Electrons",
        StoreTResult=False))

    # decorate electrons with the output of LH loose
    ElectronPassLHLoose = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,   
        name="ElectronPassLHLoose",
        EGammaElectronLikelihoodTool=ElectronLHSelectorLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHLoose",
        ContainerName="Electrons",
        StoreTResult=False))

    # decorate electrons with the output of LH loose+BL
    ElectronPassLHLooseBL = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ElectronPassLHLooseBL",
        EGammaElectronLikelihoodTool=ElectronLHSelectorLooseBL,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHLooseBL",
        ContainerName="Electrons",
        StoreTResult=False))

    # decorate electrons with the output of LH medium
    ElectronPassLHMedium = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ElectronPassLHMedium",
        EGammaElectronLikelihoodTool=ElectronLHSelectorMedium,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHMedium",
        ContainerName="Electrons",
        StoreTResult=False))

    # decorate electrons with the output of LH tight
    ElectronPassLHTight = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ElectronPassLHTight",
        EGammaElectronLikelihoodTool=ElectronLHSelectorTight,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsLHTight",
        ContainerName="Electrons",
        StoreTResult=False))

    # decorate electrons with the output of DNN Loose
    ElectronPassDNNLoose = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name = "ElectronPassDNNLoose",
        EGammaElectronLikelihoodTool = ElectronDNNSelectorLoose,
        EGammaFudgeMCTool = "",
        CutType = "",
        StoreGateEntryName = "DFCommonElectronsDNNLoose",
        ContainerName = "Electrons",
        StoreTResult = False,
        StoreGateEntryMultipleNames=["DFCommonElectronsDNN_pel",
                                     "DFCommonElectronsDNN_pcf",
                                     "DFCommonElectronsDNN_ppc",
                                     "DFCommonElectronsDNN_phf",
                                     "DFCommonElectronsDNN_ple",
                                     "DFCommonElectronsDNN_plh"],
        StoreMultipleOutputs = True))

    # decorate electrons with the output of DNN Medium
    ElectronPassDNNMedium = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name = "ElectronPassDNNMedium",
        EGammaElectronLikelihoodTool = ElectronDNNSelectorMedium,
        EGammaFudgeMCTool = "",
        CutType = "",
        StoreGateEntryName = "DFCommonElectronsDNNMedium",
        ContainerName = "Electrons",
        StoreTResult = False))

    # decorate electrons with the output of DNN Tight
    ElectronPassDNNTight = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name = "ElectronPassDNNTight",
        EGammaElectronLikelihoodTool = ElectronDNNSelectorTight,
        EGammaFudgeMCTool = "",
        CutType = "",
        StoreGateEntryName = "DFCommonElectronsDNNTight",
        ContainerName = "Electrons",
        StoreTResult = False))

    # decorate electrons with the output of ECIDS
    ElectronPassECIDS = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ElectronPassECIDS",
        EGammaElectronLikelihoodTool=ElectronChargeIDSelector,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonElectronsECIDS",
        ContainerName="Electrons",
        StoreTResult=True))

    # decorate forward electrons with the output of LH loose
    ForwardElectronPassLHLoose = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ForwardElectronPassLHLoose",
        EGammaElectronLikelihoodTool=ForwardElectronLHSelectorLoose,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonForwardElectronsLHLoose",
        ContainerName="ForwardElectrons"))

    # decorate forward electrons with the output of LH medium
    ForwardElectronPassLHMedium = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ForwardElectronPassLHMedium",
        EGammaElectronLikelihoodTool=ForwardElectronLHSelectorMedium,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonForwardElectronsLHMedium",
        ContainerName="ForwardElectrons"))

    # decorate forward electrons with the output of LH tight
    ForwardElectronPassLHTight = acc.getPrimaryAndMerge(EGElectronLikelihoodToolWrapperCfg(
        ConfigFlags,
        name="ForwardElectronPassLHTight",
        EGammaElectronLikelihoodTool=ForwardElectronLHSelectorTight,
        EGammaFudgeMCTool="",
        CutType="",
        StoreGateEntryName="DFCommonForwardElectronsLHTight",
        ContainerName="ForwardElectrons"))

    # decorate photons with the output of IsEM loose
    # on MC, fudge the shower shapes before computing the ID (but the
    # original shower shapes are not overridden)
    if isFullSim:
        PhotonPassIsEMLoose = acc.getPrimaryAndMerge(EGSelectionToolWrapperCfg(
            ConfigFlags,
            name="PhotonPassIsEMLoose",
            EGammaSelectionTool=PhotonIsEMSelectorLoose,
            EGammaFudgeMCTool=EGVariableCorrectionTool,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMLoose",
            ContainerName="Photons"))
    else:
        PhotonPassIsEMLoose = acc.getPrimaryAndMerge(EGSelectionToolWrapperCfg(
            ConfigFlags,
            name="PhotonPassIsEMLoose",
            EGammaSelectionTool=PhotonIsEMSelectorLoose,
            EGammaFudgeMCTool=None,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMLoose",
            ContainerName="Photons"))

    # decorate photons with the output of IsEM tight
    # on full-sim MC, fudge the shower shapes before computing the ID
    # (but the original shower shapes are not overridden)
    if isFullSim:
        PhotonPassIsEMTight = acc.getPrimaryAndMerge(EGSelectionToolWrapperCfg(
            ConfigFlags,
            name="PhotonPassIsEMTight",
            EGammaSelectionTool=PhotonIsEMSelectorTight,
            EGammaFudgeMCTool=EGVariableCorrectionTool,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMTight",
            ContainerName="Photons"))
    else:
        PhotonPassIsEMTight = acc.getPrimaryAndMerge(EGSelectionToolWrapperCfg(
            ConfigFlags,
            name="PhotonPassIsEMTight",
            EGammaSelectionTool=PhotonIsEMSelectorTight,
            EGammaFudgeMCTool=None,
            CutType="",
            StoreGateEntryName="DFCommonPhotonsIsEMTight",
            ContainerName="Photons"))

    # decorate photons with the output of IsEM tight pt-inclusive menu
    # Can be removed once pt-dependent cuts are fully supported.
    # On full-sim MC, fudge the shower shapes before computing the ID
    # (but the original shower shapes are not overridden)
    PhotonPassIsEMTightPtIncl = acc.getPrimaryAndMerge(EGSelectionToolWrapperCfg(
        ConfigFlags,
        name="PhotonPassIsEMTightPtIncl",
        EGammaSelectionTool=PhotonIsEMSelectorTightPtIncl,
        EGammaFudgeMCTool=(
            EGVariableCorrectionTool if isFullSim else None),
        CutType="",
        StoreGateEntryName="DFCommonPhotonsIsEMTightPtIncl",
        ContainerName="Photons"))

    # decorate photons with the photon cleaning flags
    # on MC, fudge the shower shapes before computing the flags
    from DerivationFrameworkEGamma.EGammaToolsConfig import EGPhotonCleaningWrapperCfg
    if isFullSim:
        PhotonPassCleaning = acc.getPrimaryAndMerge(EGPhotonCleaningWrapperCfg(
            ConfigFlags,
            name="PhotonPassCleaning",
            EGammaFudgeMCTool=EGVariableCorrectionTool,
            StoreGateEntryName="DFCommonPhotonsCleaning",
            ContainerName="Photons"))
    else:
        PhotonPassCleaning = acc.getPrimaryAndMerge(EGPhotonCleaningWrapperCfg(
            ConfigFlags,
            name="PhotonPassCleaning",
            EGammaFudgeMCTool=None,
            StoreGateEntryName="DFCommonPhotonsCleaning",
            ContainerName="Photons"))

    # decorate central electrons and photons with a flag to tell the the
    # candidates are affected by the crack bug in mc16a and data 2015+2016
    from DerivationFrameworkEGamma.EGammaToolsConfig import EGCrackVetoCleaningToolCfg
    PhotonPassCrackVeto = acc.getPrimaryAndMerge(EGCrackVetoCleaningToolCfg(
        ConfigFlags,
        name="PhotonPassCrackVeto",
        StoreGateEntryName="DFCommonCrackVetoCleaning",
        ContainerName="Photons"))
    ElectronPassCrackVeto = acc.getPrimaryAndMerge(EGCrackVetoCleaningToolCfg(
        ConfigFlags,
        name="ElectronPassCrackVeto",
        StoreGateEntryName="DFCommonCrackVetoCleaning",
        ContainerName="Electrons"))

    # decorate some electrons with an additional ambiguity flag
    # against internal and early material conversion
    from DerivationFrameworkEGamma.EGammaToolsConfig import EGElectronAmbiguityToolCfg
    ElectronAmbiguity = acc.getPrimaryAndMerge(EGElectronAmbiguityToolCfg(
        ConfigFlags,
        name="ElectronAdditionnalAmbiguity",
        isMC=ConfigFlags.Input.isMC))

    # list of all the decorators so far
    EGAugmentationTools = [DFCommonPhotonsDirection,
                           ElectronPassLHVeryLoose,
                           ElectronPassLHLoose,
                           ElectronPassLHLooseBL,
                           ElectronPassLHMedium,
                           ElectronPassLHTight,
                           ElectronPassDNNLoose,
                           ElectronPassDNNMedium,
                           ElectronPassDNNTight,
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
    if ConfigFlags.Input.isMC:
        simBarcodeOffset = ConfigFlags.Sim.SimBarcodeOffset

        # Decorate Electron with bkg electron type/origin
        from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg
        BkgElectronMCTruthClassifier = acc.popToolsAndMerge(MCTruthClassifierCfg(
            ConfigFlags,
            name="BkgElectronMCTruthClassifier",
            barcodeG4Shift = simBarcodeOffset+1,
            ParticleCaloExtensionTool = ""))
        acc.addPublicTool(BkgElectronMCTruthClassifier)

        from DerivationFrameworkEGamma.EGammaToolsConfig import BkgElectronClassificationCfg
        BkgElectronClassificationTool = acc.getPrimaryAndMerge(BkgElectronClassificationCfg(
            ConfigFlags,
            name="BkgElectronClassificationTool",
            MCTruthClassifierTool=BkgElectronMCTruthClassifier,
            barcodeCut=simBarcodeOffset))
        EGAugmentationTools.append(BkgElectronClassificationTool)

        # Decorate egammaTruthParticles with truth-particle-level etcone20,30,40
        from DerivationFrameworkMCTruth.TruthDerivationToolsConfig import TruthIsolationToolCfg
        TruthEgetIsolationTool = acc.getPrimaryAndMerge(TruthIsolationToolCfg(
            name="TruthEgetIsolationTool",
            isoParticlesKey="egammaTruthParticles",
            allParticlesKey="TruthParticles",
            particleIDsToCalculate=[-11, 11, 22],
            IsolationConeSizes=[0.2, 0.3, 0.4],
            excludeIDsFromCone=[-16, -14, -13, -12, 12, 13, 14, 16],
            IsolationVarNamePrefix='etcone',
            ChargedParticlesOnly=False))
        EGAugmentationTools.append(TruthEgetIsolationTool)

        # Decorate egammaTruthParticles with truth-particle-level ptcone20,30,40
        TruthEgptIsolationTool = acc.getPrimaryAndMerge(TruthIsolationToolCfg(
            name="TruthEgptIsolationTool",
            isoParticlesKey="egammaTruthParticles",
            allParticlesKey="TruthParticles",
            particleIDsToCalculate=[-11, 11, 22],
            IsolationConeSizes=[0.2, 0.3, 0.4],
            IsolationVarNamePrefix='ptcone',
            ChargedParticlesOnly=True))
        EGAugmentationTools.append(TruthEgptIsolationTool)

        # Compute the truth-particle-level energy density in the central eta region
        from EventShapeTools.EventDensityConfig import configEventDensityTool
        from JetRecConfig.JetRecConfig import getInputAlgs,getConstitPJGAlg,reOrderAlgs
        from JetRecConfig.StandardJetConstits import stdConstitDic as cst

        # Schedule PseudoJetTruth
        constit_algs = getInputAlgs(cst.Truth, configFlags=ConfigFlags)
        constit_algs = reOrderAlgs( [a for a in constit_algs if a is not None])

        for a in constit_algs:
            acc.addEventAlgo(a)
        constitPJAlg = getConstitPJGAlg(cst.Truth, suffix=None)
        acc.addEventAlgo(constitPJAlg)

        tc = configEventDensityTool("EDTruthCentralTool", cst.Truth,
                                    0.5,
                                    AbsRapidityMin=0.0,
                                    AbsRapidityMax=1.5,
                                    OutputContainer="TruthIsoCentralEventShape",
                                    OutputLevel=3
                                    )
        acc.addPublicTool(tc)

        # Compute the truth-particle-level energy density in the forward eta region
        tf = configEventDensityTool("EDTruthForwardTool", cst.Truth,
                                    0.5,
                                    AbsRapidityMin=1.5,
                                    AbsRapidityMax=3.0,
                                    OutputContainer="TruthIsoForwardEventShape",
                                    OutputLevel=3,
                                    )
        acc.addPublicTool(tf)

        acc.addEventAlgo(CompFactory.EventDensityAthAlg("EDTruthCentralAlg", EventDensityTool=tc))
        acc.addEventAlgo(CompFactory.EventDensityAthAlg("EDTruthForwardAlg", EventDensityTool=tf))

    # =======================================
    # CREATE THE DERIVATION KERNEL ALGORITHM
    # =======================================

    acc.addEventAlgo(CompFactory.DerivationFramework.CommonAugmentation(
        "EGammaCommonKernel",
        AugmentationTools=EGAugmentationTools
    ))

    # =======================================
    # ADD TOOLS : custom electron, photon and muon track isolation
    # =======================================
    from IsolationAlgs.DerivationTrackIsoConfig import DerivationTrackIsoCfg
    acc.merge(DerivationTrackIsoCfg(ConfigFlags, object_type = ('Electrons', 'Muons')))

    return acc

