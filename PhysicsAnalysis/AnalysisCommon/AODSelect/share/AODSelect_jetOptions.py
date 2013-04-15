
# Only include this file once
include.block ("AODSelect/AODSelect_jetOptions.py")


# =============================================================================
# Get or create all needed tools
# =============================================================================

# ----------------------------
# Clean jet flagging
# ----------------------------

# Get the very loose jet cleaning tool for flagging jets
if AODSelectFlags.doVeryLooseCleanJetFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_VeryLoose"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_VeryLoose
        ToolSvc += ConfiguredAthJetCleaningTool_VeryLoose("AODSelect_ConfiguredAthJetCleaningTool_VeryLoose")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doVeryLooseCleanJetFlagging, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_VeryLoose )
    pass

# Get the loose jet cleaning tool for flagging jets
if AODSelectFlags.doLooseCleanJetFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_Loose"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_Loose
        ToolSvc += ConfiguredAthJetCleaningTool_Loose("AODSelect_ConfiguredAthJetCleaningTool_Loose")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doLooseCleanJetFlagging, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_Loose )
    pass

# Get the medium jet cleaning tool for flagging jets
if AODSelectFlags.doMediumCleanJetFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_Medium"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_Medium
        ToolSvc += ConfiguredAthJetCleaningTool_Medium("AODSelect_ConfiguredAthJetCleaningTool_Medium")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doMediumCleanJetFlagging, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_Medium )
    pass

# Get the tight jet cleaning tool for flagging jets
if AODSelectFlags.doTightCleanJetFlagging():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_Tight"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_Tight
        ToolSvc += ConfiguredAthJetCleaningTool_Tight("AODSelect_ConfiguredAthJetCleaningTool_Tight")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doTightCleanJetFlagging, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_Tight )
    pass



# ----------------------------
# Clean jet building
# ----------------------------

# Get the very loose jet cleaning tool for building new jet (pointer/link) collections
if AODSelectFlags.doVeryLooseCleanJetBuilding():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_VeryLoose"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_VeryLoose
        ToolSvc += ConfiguredAthJetCleaningTool_VeryLoose("AODSelect_ConfiguredAthJetCleaningTool_VeryLoose")
        pass
    configurationHelper.addSelectorTool( AODSelectFlags.doVeryLooseCleanJetBuilding, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_VeryLoose )
    pass

# Get the loose jet cleaning tool for building new jet (pointer/link) collections
if AODSelectFlags.doLooseCleanJetBuilding():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_Loose"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_Loose
        ToolSvc += ConfiguredAthJetCleaningTool_Loose("AODSelect_ConfiguredAthJetCleaningTool_Loose")
        pass
    configurationHelper.addSelectorTool( AODSelectFlags.doLooseCleanJetBuilding, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_Loose )
    pass

# Get the medium jet cleaning tool for building new jet (pointer/link) collections
if AODSelectFlags.doMediumCleanJetBuilding():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_Medium"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_Medium
        ToolSvc += ConfiguredAthJetCleaningTool_Medium("AODSelect_ConfiguredAthJetCleaningTool_Medium")
        pass
    configurationHelper.addSelectorTool( AODSelectFlags.doMediumCleanJetBuilding, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_Medium )
    pass

# Get the tight jet cleaning tool for building new jet (pointer/link) collections
if AODSelectFlags.doTightCleanJetBuilding():
    if not hasattr(ToolSvc, "AODSelect_ConfiguredAthJetCleaningTool_Tight"):
        from JetSelectorTools.ConfiguredAthJetCleaningTools import ConfiguredAthJetCleaningTool_Tight
        ToolSvc += ConfiguredAthJetCleaningTool_Tight("AODSelect_ConfiguredAthJetCleaningTool_Tight")
        pass
    configurationHelper.addSelectorTool( AODSelectFlags.doTightCleanJetBuilding, ToolSvc.AODSelect_ConfiguredAthJetCleaningTool_Tight )
    pass

