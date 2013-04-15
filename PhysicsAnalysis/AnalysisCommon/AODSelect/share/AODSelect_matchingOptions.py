
# Only include this file once
include.block ("AODSelect/AODSelect_matchingOptions.py")


# =============================================================================
# Get or create all needed tools
# =============================================================================

# ----------------------------
# Set up the DeltaR matching tools for offline-to-offline matching
# ----------------------------
if AODSelectFlags.doOfflineMatching():
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    matchDict = AODSelectFlags.doOfflineMatching.matchingDictionary
    logAODSelect_mainOptions.debug( "We got the following offline matching dictionary for offline matching: %s" % matchDict )
    for matchFromKey in matchDict.keys():
        logAODSelect_mainOptions.debug( "We got the following offline matchFrom inputTypeAndKey: %s" % matchFromKey )
        toolList = []
        for matchToValueAndDistance in matchDict[matchFromKey]:
            matchToValue = matchToValueAndDistance[0]
            distance     = matchToValueAndDistance[1]
            distanceName = ("deltaR%3.2f" % distance).replace(".","")
            logAODSelect_mainOptions.debug( "We got the following offline matchTo inputTypeAndKey=%s and distance=%s"
                                            % (matchToValue, distance) )
            if matchToValue.__contains__("#"): matchToValue = matchToValue.split("#")[1]
            toolName = "AODSelect_DeltaRAssociationTool__"+distanceName+"_"+matchToValue
            if not hasattr(ToolSvc, toolName):
                ToolSvc += DeltaRAssociationTool( toolName,
                                                  OutputLevel                = INFO,
                                                  inputAssociateToCollection = matchToValue,
                                                  deltaRMax                  = distance
                                                  )
                pass
            toolList.append( getattr(ToolSvc, toolName) )
            pass
        configurationHelper.addAssociationToolList( matchFromKey, toolList )
        pass
    pass





# ----------------------------
# Set up the trigger matching tools for offline-to-trigger matching
# ----------------------------
if AODSelectFlags.doTriggerMatching():
    from AssociationComps.AssociationCompsConf import TriggerAssociationTool
    matchDict = AODSelectFlags.doTriggerMatching.matchingDictionary
    logAODSelect_mainOptions.debug( "We got the following trigger matching dictionary for trigger matching: %s" % matchDict )
    for matchFromKey in matchDict.keys():
        logAODSelect_mainOptions.debug( "We got the following trigger matchFrom inputTypeAndKey: %s" % matchFromKey )
        toolList = []
        for triggerNameAndDistance in matchDict[matchFromKey]:
            triggerName  = triggerNameAndDistance[0]
            distance     = triggerNameAndDistance[1]
            distanceName = ("deltaR%3.2f" % distance).replace(".","")
            logAODSelect_mainOptions.debug( "We got the following trigger name to match to: %s, with a distance=%s"
                                            % (triggerName, distance) )
            if triggerName.__contains__("#"): triggerName = triggerName.split("#")[1]
            toolName = "AODSelect_TriggerAssociationTool__"+distanceName+"_"+triggerName
            if not hasattr(ToolSvc, toolName):
                ToolSvc += TriggerAssociationTool( toolName,
                                                   OutputLevel               = INFO,
                                                   triggerToMatchTo          = triggerName,
                                                   deltaR                    = distance,
                                                   matchToPassedTriggersOnly = AODSelectFlags.doTriggerMatching.matchToPassedTriggersOnly
                                                   )
                pass
            toolList.append( getattr(ToolSvc, toolName) )
            pass
        configurationHelper.addAssociationToolList( matchFromKey, toolList )
        pass
    pass




# ----------------------------
# Set up the truth matching tools for offline-to-truth matching
# ----------------------------
if AODSelectFlags.isSim() and AODSelectFlags.doTruthMatching():
    from AssociationComps.AssociationCompsConf import MCTruthClassifierAssociationTool
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as AODSelectMCTruthClassifier
    AODSelectMCTruthClassifier.McEventCollection = AODSelectFlags.mcEventCollection()
    
    matchDict = AODSelectFlags.doTruthMatching.matchingDictionary
    logAODSelect_mainOptions.debug( "We got the following truth matching dictionary for truth matching: %s" % matchDict )
    for matchFromKey in matchDict.keys():
        logAODSelect_mainOptions.debug( "We got the following truth matchFrom inputTypeAndKey: %s" % matchFromKey )
        toolName = "AODSelect_MCTruthClassifierAssociationTool__"+AODSelectFlags.truthParticleContainer()
        if not hasattr(ToolSvc, toolName):
            ToolSvc += MCTruthClassifierAssociationTool( toolName,
                                                         OutputLevel                = INFO,
                                                         mcTruthClassifier          = AODSelectMCTruthClassifier,
                                                         inputAssociateToCollection = AODSelectFlags.truthParticleContainer(),
                                                         writeUserData              = AODSelectFlags.doTruthMatching.writeUserData
                                                         )
            pass
        configurationHelper.addAssociationTool( matchFromKey, getattr(ToolSvc, toolName) )
        pass
    pass

