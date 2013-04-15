
# Only include this file once
include.block ("AODSelect/AODSelect_doAllOptions.py")


# =============================================================================
# Common job preparation
# =============================================================================
from AthenaCommon.Logging import logging
logAODSelect_doAllOptions = logging.getLogger( 'AODSelect_doAllOptions' )
#logAODSelect_doAllOptions.setLevel(DEBUG)

# Import the AODSelectFlags to steer the job
from AODSelect.AODSelectFlags import AODSelectFlags

# Print a debug message
logAODSelect_doAllOptions.debug( "Starting with file AODSelect/AODSelect_doAllOptions.py" )


# =============================================================================
# Define the default items to use
# =============================================================================
photonCollList   = [ "PhotonContainer#PhotonAODCollection" ]
electronCollList = [ "ElectronContainer#ElectronAODCollection" ]
muonCollList     = [ "Analysis::MuonContainer#Muons",
                     "Analysis::MuonContainer#StacoMuonCollection",
                     "Analysis::MuonContainer#MuidMuonCollection",
                     "Analysis::MuonContainer#CaloMuonCollection" ]
tauCollList      = [ "Analysis::TauJetContainer#TauRecContainer" ]
jetCollList      = [ "JetCollection#AntiKt4TopoEMJets",
                     "JetCollection#AntiKt6TopoEMJets",
                     "JetCollection#AntiKt4LCTopoJets",
                     "JetCollection#AntiKt6LCTopoJets" ]



# =============================================================================
# Fill appropriate flags
# =============================================================================

# -----------------------------
# Fill the flagging flags
# -----------------------------
for jetColl in jetCollList:
    AODSelectFlags.doVeryLooseCleanJetFlagging.add( inputItem = jetColl )
    AODSelectFlags.doLooseCleanJetFlagging.add( inputItem = jetColl )
    AODSelectFlags.doMediumCleanJetFlagging.add( inputItem = jetColl )
    AODSelectFlags.doTightCleanJetFlagging.add( inputItem = jetColl )
    pass


# -----------------------------
# Fill the building flags
# -----------------------------
for jetColl in jetCollList:
    AODSelectFlags.doVeryLooseCleanJetBuilding.add( inputItem                  = jetColl,
                                                    createOutputCollection     = True,
                                                    createOutputLinkCollection = True )
    AODSelectFlags.doLooseCleanJetBuilding.add( inputItem                  = jetColl,
                                                createOutputCollection     = True,
                                                createOutputLinkCollection = True )
    AODSelectFlags.doMediumCleanJetBuilding.add( inputItem                  = jetColl,
                                                 createOutputCollection     = True,
                                                 createOutputLinkCollection = True )
    AODSelectFlags.doTightCleanJetBuilding.add( inputItem                  = jetColl,
                                                createOutputCollection     = True,
                                                createOutputLinkCollection = True )
    pass



# -----------------------------
# Fill the matching flags
# -----------------------------
for photonColl in photonCollList:
    combinedList = muonCollList + tauCollList + jetCollList
    logAODSelect_doAllOptions.debug( "Scheduling offline matching between inputItem=%s and matchToItemList=%s"
                                     % (photonColl, combinedList) )
    AODSelectFlags.doOfflineMatching.add( inputItem       = photonColl,
                                          matchToItemList = combinedList )
    pass
for electronColl in electronCollList:
    combinedList = muonCollList + tauCollList + jetCollList
    logAODSelect_doAllOptions.debug( "Scheduling offline matching between inputItem=%s and matchToItemList=%s"
                                     % (electronColl, combinedList) )
    AODSelectFlags.doOfflineMatching.add( inputItem       = electronColl,
                                          matchToItemList = combinedList )
    pass
for muonColl in muonCollList:
    combinedList = electronCollList + photonCollList + tauCollList + jetCollList
    logAODSelect_doAllOptions.debug( "Scheduling offline matching between inputItem=%s and matchToItemList=%s"
                                     % (muonColl, combinedList) )
    AODSelectFlags.doOfflineMatching.add( inputItem       = muonColl,
                                          matchToItemList = combinedList )
    pass
for tauColl in tauCollList:
    combinedList = electronCollList + photonCollList + muonCollList + jetCollList
    logAODSelect_doAllOptions.debug( "Scheduling offline matching between inputItem=%s and matchToItemList=%s"
                                     % (tauColl, combinedList) )
    AODSelectFlags.doOfflineMatching.add( inputItem       = tauColl,
                                          matchToItemList = combinedList )
    pass
for jetColl in jetCollList:
    logAODSelect_doAllOptions.debug( "Scheduling offline matching between inputItem=%s and matchToItemList"
                                     % (jetColl) )
    combinedList = electronCollList + photonCollList + tauCollList + muonCollList
    logAODSelect_doAllOptions.debug( "Scheduling offline matching between inputItem=%s and matchToItemList=%s"
                                     % (jetColl, combinedList) )
    AODSelectFlags.doOfflineMatching.add( inputItem       = jetColl,
                                          matchToItemList = combinedList )
    pass



# Print a debug message
logAODSelect_doAllOptions.debug( "Done with file AODSelect/AODSelect_doAllOptions.py" )
