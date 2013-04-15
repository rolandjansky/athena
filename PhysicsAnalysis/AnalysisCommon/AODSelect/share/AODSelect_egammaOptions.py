
# Only include this file once
include.block ("AODSelect/AODSelect_egammaOptions.py")



# =============================================================================
# Get or create all needed tools
# =============================================================================


# ----------------------------
# egamma calo isolation correction
# ----------------------------
if AODSelectFlags.doEGammaCaloIsolationFlagging():
    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    from PhotonD3PDMaker.PAUCaloIsolationTool import PAUCaloIsolationTool
    if not hasattr(ToolSvc, "AODSelect_EGammaIsolationCorrectionCalcTool"):
        from UserDataUtils.UserDataUtilsConf import EGammaIsolationCorrectionCalcTool
        ToolSvc += EGammaIsolationCorrectionCalcTool("AODSelect_EGammaIsolationCorrectionCalcTool",
                                                     RemoveNHardestJets   = PAUflags.JetAreaJetsToRemove(),
                                                     PAUcaloIsolationTool = PAUCaloIsolationTool,
                                                     isolationNameList    = AODSelectFlags.doEGammaCaloIsolationFlagging.varNameList
                                                     )
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doEGammaCaloIsolationFlagging, ToolSvc.AODSelect_EGammaIsolationCorrectionCalcTool )
    pass


# ----------------------------
# electron selector flagging
# ----------------------------

# Get the MultiLepton electron selector tool for flagging electrons
if AODSelectFlags.doElectronMultiLeptonIDFlagging():
    if not hasattr(ToolSvc, "AODSelect_AthElectronMultiLeptonSelector"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronMultiLeptonSelector
        ToolSvc += AthElectronMultiLeptonSelector("AODSelect_AthElectronMultiLeptonSelector")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronMultiLeptonIDFlagging, ToolSvc.AODSelect_AthElectronMultiLeptonSelector )
    pass

# Get the electron likelihood tool and create an instance for each cut level
if AODSelectFlags.doElectronLikelihoodIDFlagging_VeryLoose():
    if not hasattr(ToolSvc, "AODSelect_AthElectronLikelihoodSelector_VeryLoose"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
        ToolSvc += AthElectronLikelihoodTool( "AODSelect_AthElectronLikelihoodSelector_VeryLoose",
                                              inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                              cutLikelihoodEnum = 0 ) #FIXME: The enums need to be standardised
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronLikelihoodIDFlagging_VeryLoose,
                                         ToolSvc.AODSelect_AthElectronLikelihoodSelector_VeryLoose )
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_Loose():
    if not hasattr(ToolSvc, "AODSelect_AthElectronLikelihoodSelector_Loose"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
        ToolSvc += AthElectronLikelihoodTool( "AODSelect_AthElectronLikelihoodSelector_Loose",
                                              inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                              cutLikelihoodEnum = 1 ) #FIXME: The enums need to be standardised
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronLikelihoodIDFlagging_Loose,
                                         ToolSvc.AODSelect_AthElectronLikelihoodSelector_Loose )
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_Medium():
    if not hasattr(ToolSvc, "AODSelect_AthElectronLikelihoodSelector_Medium"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
        ToolSvc += AthElectronLikelihoodTool( "AODSelect_AthElectronLikelihoodSelector_Medium",
                                              inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                              cutLikelihoodEnum = 2 ) #FIXME: The enums need to be standardised
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronLikelihoodIDFlagging_Medium,
                                         ToolSvc.AODSelect_AthElectronLikelihoodSelector_Medium )
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_Tight():
    if not hasattr(ToolSvc, "AODSelect_AthElectronLikelihoodSelector_Tight"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
        ToolSvc += AthElectronLikelihoodTool( "AODSelect_AthElectronLikelihoodSelector_Tight",
                                              inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                              cutLikelihoodEnum = 3 ) #FIXME: The enums need to be standardised
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronLikelihoodIDFlagging_Tight,
                                         ToolSvc.AODSelect_AthElectronLikelihoodSelector_Tight )
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_VeryTight():
    if not hasattr(ToolSvc, "AODSelect_AthElectronLikelihoodSelector_VeryTight"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
        ToolSvc += AthElectronLikelihoodTool( "AODSelect_AthElectronLikelihoodSelector_VeryTight",
                                              inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                              cutLikelihoodEnum = 4 ) #FIXME: The enums need to be standardised
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronLikelihoodIDFlagging_VeryTight,
                                         ToolSvc.AODSelect_AthElectronLikelihoodSelector_VeryTight )
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_Loose_BL_Pix():
    if not hasattr(ToolSvc, "AODSelect_AthElectronLikelihoodSelector_Loose_BL_Pix"):
        from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
        ToolSvc += AthElectronLikelihoodTool( "AODSelect_AthElectronLikelihoodSelector_Loose_BL_Pix",
                                              inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                              cutLikelihoodEnum = 100 ) #FIXME: The enums need to be standardised
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doElectronLikelihoodIDFlagging_Loose_BL_Pix,
                                         ToolSvc.AODSelect_AthElectronLikelihoodSelector_Loose_BL_Pix )
    pass

