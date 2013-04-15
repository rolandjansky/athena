
# Only include this file once
include.block ("AODSelect/AODSelect_muonOptions.py")


# =============================================================================
# Get or create all needed tools
# =============================================================================


# ----------------------------
# muon selector flagging
# ----------------------------

# Get the muon selector tool for flagging muons
if AODSelectFlags.doMuonSelectorFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthMuonSelector"):
        from MuonSelectorTools.ConfiguredAthMuonSelectors import ConfiguredAthMuonSelector
        ToolSvc += ConfiguredAthMuonSelector("AODSelect_ConfiguredAthMuonSelector")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doMuonSelectorFlagging, ToolSvc.AODSelect_ConfiguredAthMuonSelector )
    pass

# Get the muon selector tool for flagging calo muons
if AODSelectFlags.doCaloMuonSelectorFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthMuonSelector_Calo"):
        from MuonSelectorTools.ConfiguredAthMuonSelectors import ConfiguredAthMuonSelector_Calo
        ToolSvc += ConfiguredAthMuonSelector_Calo("AODSelect_ConfiguredAthMuonSelector_Calo")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doCaloMuonSelectorFlagging, ToolSvc.AODSelect_ConfiguredAthMuonSelector_Calo )
    pass

# Get the muon selector tool for flagging stand-alone muons
if AODSelectFlags.doStandAloneMuonSelectorFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthMuonSelector_StandAlone"):
        from MuonSelectorTools.ConfiguredAthMuonSelectors import ConfiguredAthMuonSelector_StandAlone
        ToolSvc += ConfiguredAthMuonSelector_StandAlone("AODSelect_ConfiguredAthMuonSelector_StandAlone")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doStandAloneMuonSelectorFlagging, ToolSvc.AODSelect_ConfiguredAthMuonSelector_StandAlone )
    pass


