##-----------------------------------------------------------------------------
## Name: D2PD_H4lBremRec.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the DAOD_ELLOOSE18
##
##-----------------------------------------------------------------------------

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags
D2PDFlags.WriteDAOD_H4lBremRecStream = True

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

# for messaging
from AthenaCommon.Logging import logging
h4lDPD_msg = logging.getLogger( 'DAOD_H4L' )

# Import the configuration manager that knows about all algorithms and tools
from AthenaCommon import CfgMgr

# Import the tool service that will hold all tool instances
from AthenaCommon.AppMgr import ToolSvc

# Import the service manager that will hold all instances of services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Needed algorithms and tools
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner


#---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
#import PyUtils.RootUtils as ru
#ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters


# ====================================================================
# Define the test DPD output stream
# ====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
fileName   = buildFileName( D2PDFlags.WriteDAOD_H4lBremRecStream )
streamName = D2PDFlags.WriteDAOD_H4lBremRecStream.StreamName




# ====================================================================
# Check if we have Monte Carlo or real data, based on the inputFileSummary
# ====================================================================
if inputFileSummary.has_key("evt_type"):
    eventTypeList = inputFileSummary.get("evt_type")
    if eventTypeList.__contains__("IS_SIMULATION") :
        print "Detected that the input file is a simulated dataset"
        D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation = True
        pass
    pass

# Check for the presence of GEN_AOD, TruthEvent (ESD), GEN_EVENT, and SpclMC:
hasSpclMC = False
mcEventCollectionForMCTruthClassifier = ""
truthPartCollectionForMCTruthClassifier = ""
if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation :
    mcEventColls = ""
    if inputFileSummary.has_key("eventdata_itemsDic"):
        eventDic = inputFileSummary.get('eventdata_itemsDic')
        mcEventColls = eventDic['McEventCollection']
        if mcEventColls.__contains__("GEN_AOD") :
            mcEventCollectionForMCTruthClassifier = "GEN_AOD"
            pass
        elif mcEventColls.__contains__("TruthEvent") :
            mcEventCollectionForMCTruthClassifier = "TruthEvent"
            pass
        elif mcEventColls.__contains__("GEN_EVENT") :
            mcEventCollectionForMCTruthClassifier = "GEN_EVENT"
            pass
        truthPartColls = eventDic['TruthParticleContainer']
        if truthPartColls.__contains__("SpclMC") :
            truthPartCollectionForMCTruthClassifier = "SpclMC"
            hasSpclMC = True
            pass
        pass
    # Print a error message if no MCEventCollection is found
    if mcEventCollectionForMCTruthClassifier == "" :
        h4lDPD_msg.error( "No MCEventCollection found even though the input file is simulation!" )
        h4lDPD_msg.error( "These are the McEventCollections from the input file: %s" % mcEventColls )
        pass
    pass



# ====================================================================
# Run the electron GSF brem fit
# ====================================================================
#include("egammaBremRec/D2PDMakerElectronPostProcessing.py")
if D2PDFlags.WriteDAOD_H4lBremRecStream.DoBremRec :
    #from egammaBremRec.egammaBremGetter import egammaBremGetter
    #egammaBremGetter(ignoreExistingDataObject=True)
    h4lDPD_msg.info( "Rerunning GSF egamma track refit" )
    pass



#====================================================================
# Define the event selection
#====================================================================
# List of algs to be used for final accept event decision
algsOR = []

#====================================================================
# Create a list of algorithms to bookkeep and matchings to use
#====================================================================
h4lAlgsToBookkeep = []
h4lElAssociationToolList = []
h4lElOutputAssociationContainerList = []
h4lMuAssociationToolList = []
h4lMuOutputAssociationContainerList = []


#====================================================================
# Do the MC truth stuff
#====================================================================
if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation:
    # Truth pre-selector (this way, only one loop over the SpclMC container is needed per event -> faster)
    from D2PDMaker.D2PDMakerConf import D2PDTruthParticleSelector
    topSequence += D2PDTruthParticleSelector( "TruthPreSelectorInH4lBremRecoStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'SpclMC',
                                              outputLinkCollection    = 'TruthParticlePreSelectedLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 4, 5, 6, 11, 12, 13, 14, 15, 16, 22, 23, 24, 25 ], # c, b, t, e, nu_e, mu, nu_mu, tau, nu_tau, gamma, Z, W, H
                                              removeDocumentationLine = False,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    h4lAlgsToBookkeep.append( "TruthPreSelectorInH4lBremRecoStream" )

    # Truth-electron selector:
    topSequence += D2PDTruthParticleSelector( "TruthLeptonSelectorInH4lBremRecoStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthLeptonFromBosonLinkCollection',
                                              minNumberPassed         = 2,
                                              pdgIDList               = [ 11, 13 ], # e-, e+, mu+, mu-
                                              pdgIDListOrigin         = [ 23, 24, 25 ], # Z, W, H boson
                                              acceptStatusCodeList    = [ 1 ], # stable
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = True,
                                              removeSelfDecay         = True
                                              )
    h4lAlgsToBookkeep.append( "TruthLeptonSelectorInH4lBremRecoStream" )

    # Truth Z0/gamma* selector:
    topSequence += D2PDTruthParticleSelector( "TruthZGammaBosonSelectorInH4lBremRecoStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthZGammaBosonLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 22, 23 ], # gamma, Z boson
                                              pdgIDListOrigin         = [ 25 ], # H boson
                                              #acceptStatusCodeList    = [ 2, 10902 ],
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    h4lAlgsToBookkeep.append( "TruthZGammaBosonSelectorInH4lBremRecoStream" )

    # Truth Z0 selector:
    topSequence += D2PDTruthParticleSelector( "TruthZBosonSelectorInH4lBremRecoStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthZBosonLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 23 ], # Z boson
                                              #acceptStatusCodeList    = [ 2, 10902 ],
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    h4lAlgsToBookkeep.append( "TruthZBosonSelectorInH4lBremRecoStream" )

    # Truth W selector:
    topSequence += D2PDTruthParticleSelector( "TruthWBosonSelectorInH4lBremRecoStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthHiggsBosonLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 25 ], # Higgs boson
                                              #acceptStatusCodeList    = [ 2, 10902 ],
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    h4lAlgsToBookkeep.append( "TruthHiggsBosonSelectorInH4lBremRecoStream" )

    # Build a composite Z boson out of the true leptons
    topSequence += D2PDParticleCombiner( "TruthZBosonCombinerInH4lBremRecoStream",
                                         OutputLevel                     = INFO,
                                         compositeParticlePDG_ID         = 23, # This is a Z Boson
                                         inputCollectionList             = [ 'TruthLeptonFromBosonLinkCollection',
                                                                             'TruthLeptonFromBosonLinkCollection' ],
                                         outputCollection                = 'TruthZBosonCollection',
                                         minNumberPassed                 = 1,
                                         mcTruthRequireSameMotherPdgID   = True,
                                         mcTruthRequireSameMotherBarcode = True
                                         )
    h4lAlgsToBookkeep.append( "TruthZBosonCombinerInH4lBremRecoStream" )

    # Build a composite higgs boson out of the composite Z bosons that are made of true leptons
    topSequence += D2PDParticleCombiner( "TruthHiggsBosonCombinerInH4lBremRecoStream",
                                         OutputLevel                     = INFO,
                                         compositeParticlePDG_ID         = 23, # This is a Z Boson
                                         inputCollectionList             = [ 'TruthZBosonCollection',
                                                                             'TruthZBosonCollection' ],
                                         outputCollection                = 'TruthHiggsBosonCollection',
                                         minNumberPassed                 = 1
                                         )
    h4lAlgsToBookkeep.append( "TruthHiggsBosonCombinerInH4lBremRecoStream" )

    # Build a composite higgs boson out of the true Z/gamma bosons 
    topSequence += D2PDParticleCombiner( "TruthHiggsFromTrueZBosonCombinerInH4lBremRecoStream",
                                         OutputLevel                     = INFO,
                                         compositeParticlePDG_ID         = 23, # This is a Z Boson
                                         inputCollectionList             = [ 'TruthZGammaBosonLinkCollection',
                                                                             'TruthZGammaBosonLinkCollection' ],
                                         outputCollection                = 'TruthHiggsFromTrueZBosonCollection',
                                         minNumberPassed                 = 1,
                                         #mcTruthRequireSameMotherPdgID   = True, # Kills all... not sure why
                                         mcTruthRequireSameMotherBarcode = True
                                         )
    h4lAlgsToBookkeep.append( "TruthHiggsFromTrueZBosonCombinerInH4lBremRecoStream" )



    # Truth matching/classification using the MCTruthClassifier
    # First, set up the standard MCTruthClassifier
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as LeptonMCTruthClassifierInH4lBremRecoStream
    LeptonMCTruthClassifierInH4lBremRecoStream.McEventCollection = mcEventCollectionForMCTruthClassifier
    LeptonMCTruthClassifierInH4lBremRecoStream.OutputLevel       = ERROR

    # Now, use the MCTruthClassifier for the association/matching tool
    from AssociationComps.AssociationCompsConf import MCTruthClassifierAssociationTool
    ToolSvc += MCTruthClassifierAssociationTool( "LeptonMCTruthClassifierAssociationToolInH4lBremRecoStream",
                                                 OutputLevel                      = INFO,
                                                 mcTruthClassifier                = LeptonMCTruthClassifierInH4lBremRecoStream,
                                                 inputAssociateToCollection       = "SpclMC",
                                                 userDataPrefix                   = "truthMatch_",
                                                 userDataMatchParticleTypeName    = "particleType",
                                                 userDataMatchParticleOriginName  = "particleOrigin",
                                                 userDataMatchParticleOutcomeName = "particleOutcome",
                                                 writeUserData                    = True
                                                 )
    
    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    ToolSvc += DeltaRAssociationTool( "LeptonDeltaR02TruthAssociationToolInH4lBremRecoStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthLeptonFromBosonLinkCollection",
                                      deltaRMax                  = 0.2,
                                      userDataPrefix             = "lep_deltaR02MatchTruth_",
                                      writeUserData              = True
                                      )

    pass #End: if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation





# ====================================================================
# Do the matching via AODSelect
# ====================================================================
from AODSelect.AODSelectFlags import AODSelectFlags
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyPhotonTruthMatching :
    AODSelectFlags.doTruthMatching.add( inputItem = "PhotonContainer#PhotonAODCollection" )
    pass
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyElectronTruthMatching :
    AODSelectFlags.doTruthMatching.add( inputItem = "ElectronContainer#ElectronAODCollection" )
    pass
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyMuonTruthMatching :
    AODSelectFlags.doTruthMatching.add( inputItem = "Analysis::MuonContainer#CaloMuonCollection" )
    AODSelectFlags.doTruthMatching.add( inputItem = "Analysis::MuonContainer#StacoMuonCollection" )
    AODSelectFlags.doTruthMatching.add( inputItem = "Analysis::MuonContainer#Muons" )
    pass
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyTrackCaloClusterMatching :
    AODSelectFlags.doOfflineMatching.add( inputItem         = "Rec::TrackParticleContainer#TrackParticleCandidate",
                                          matchToItemList   = [ "CaloClusterContainer#CaloCalTopoCluster" ],
                                          matchDistanceList = [ 0.5 ] )
    pass
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyCaloClusterTrackMatching :
    AODSelectFlags.doOfflineMatching.add( inputItem         = "CaloClusterContainer#CaloCalTopoCluster",
                                          matchToItemList   = [ "Rec::TrackParticleContainer#TrackParticleCandidate" ],
                                          matchDistanceList = [ 0.5 ] )
    pass


# ====================================================================
# Do jet cleaning and building via AODSelect
# ====================================================================
AODSelectFlags.doVeryLooseCleanJetBuilding.add( inputItem                  = "JetCollection#AntiKt4TopoEMJets",
                                                createOutputCollection     = True,
                                                createOutputLinkCollection = True )
AODSelectFlags.doVeryLooseCleanJetBuilding.add( inputItem                  = "JetCollection#AntiKt4LCTopoJets",
                                                createOutputCollection     = True,
                                                createOutputLinkCollection = True )

AODSelectFlags.doVeryLooseCleanJetFlagging.add(  inputItem = "JetCollection#AntiKt4TopoEMJets" )
AODSelectFlags.doLooseCleanJetFlagging.add(  inputItem = "JetCollection#AntiKt4TopoEMJets" )
AODSelectFlags.doMediumCleanJetFlagging.add( inputItem = "JetCollection#AntiKt4TopoEMJets" )
AODSelectFlags.doTightCleanJetFlagging.add(  inputItem = "JetCollection#AntiKt4TopoEMJets" )

AODSelectFlags.doVeryLooseCleanJetFlagging.add(  inputItem = "JetCollection#AntiKt4LCTopoJets" )
AODSelectFlags.doLooseCleanJetFlagging.add(  inputItem = "JetCollection#AntiKt4LCTopoJets" )
AODSelectFlags.doMediumCleanJetFlagging.add( inputItem = "JetCollection#AntiKt4LCTopoJets" )
AODSelectFlags.doTightCleanJetFlagging.add(  inputItem = "JetCollection#AntiKt4LCTopoJets" )

# ====================================================================
# Calculate some more variables via AODSelect
# ====================================================================
AODSelectFlags.doImpactParameterFlagging.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doImpactParameterFlagging.add( inputItem = "Analysis::MuonContainer#CaloMuonCollection" )
AODSelectFlags.doImpactParameterFlagging.add( inputItem = "Analysis::MuonContainer#StacoMuonCollection" )
AODSelectFlags.doImpactParameterFlagging.add( inputItem = "Analysis::MuonContainer#Muons" )

AODSelectFlags.doEGammaCaloIsolationFlagging.add( inputItem   = "ElectronContainer#ElectronAODCollection",
                                                  varNameList = ["topoetcone20_corrected", "etcone20_NPVcorrected"] )

AODSelectFlags.doMuonSelectorFlagging.add( inputItem = "Analysis::MuonContainer#StacoMuonCollection" )
AODSelectFlags.doMuonSelectorFlagging.add( inputItem = "Analysis::MuonContainer#Muons" )
AODSelectFlags.doStandAloneMuonSelectorFlagging.add( inputItem = "Analysis::MuonContainer#StacoMuonCollection" )
AODSelectFlags.doStandAloneMuonSelectorFlagging.add( inputItem = "Analysis::MuonContainer#Muons" )
AODSelectFlags.doCaloMuonSelectorFlagging.add( inputItem = "Analysis::MuonContainer#CaloMuonCollection" )

AODSelectFlags.doElectronMultiLeptonIDFlagging.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doElectronLikelihoodIDFlagging_VeryLoose.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doElectronLikelihoodIDFlagging_Loose.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doElectronLikelihoodIDFlagging_Medium.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doElectronLikelihoodIDFlagging_Tight.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doElectronLikelihoodIDFlagging_VeryTight.add( inputItem = "ElectronContainer#ElectronAODCollection" )
AODSelectFlags.doElectronLikelihoodIDFlagging_Loose_BL_Pix.add( inputItem = "ElectronContainer#ElectronAODCollection" )



#====================================================================
# Do the trigger selection/flagging/matching
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from AssociationComps.AssociationCompsConf import TriggerAssociationTool
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for triggerName in D2PDFlags.WriteDAOD_H4lBremRecStream.MuonTriggerList:
    # Do the filtering/flagging
    filterName = "TriggerFilterInH4lBremRecoStream_" + triggerName
    topSequence += TriggerFilter( filterName,
                                  trigger = triggerName
                                  )
    h4lAlgsToBookkeep.append( filterName )
    
    # Also, add the trigger associations
    AODSelectFlags.doTriggerMatching.add( inputItem         = ("Analysis::MuonContainer#StacoMuonCollection"),
                                          matchToItemList   = [ triggerName ],
                                          matchDistanceList = [ 0.15 ] )
    AODSelectFlags.doTriggerMatching.add( inputItem         = ("Analysis::MuonContainer#Muons"),
                                          matchToItemList   = [ triggerName ],
                                          matchDistanceList = [ 0.15 ] )
    pass

for triggerName in D2PDFlags.WriteDAOD_H4lBremRecStream.ElectronTriggerList:
    # Do the filtering/flagging
    filterName = "TriggerFilterInH4lBremRecoStream_" + triggerName
    topSequence += TriggerFilter( filterName,
                                  trigger = triggerName
                                  )
    h4lAlgsToBookkeep.append( filterName )
    
    # Also, add the trigger associations
    AODSelectFlags.doTriggerMatching.add( inputItem         = "ElectronContainer#ElectronAODCollection",
                                          matchToItemList   = [ triggerName ],
                                          matchDistanceList = [ 0.15 ] )
    pass

for triggerName in D2PDFlags.WriteDAOD_H4lBremRecStream.ElectronMuonTriggerList:
    # Do the filtering/flagging
    filterName = "TriggerFilterInH4lBremRecoStream_" + triggerName
    topSequence += TriggerFilter( filterName,
                                  trigger = triggerName
                                  )
    h4lAlgsToBookkeep.append( filterName )
    
    # Also, add the trigger associations
    AODSelectFlags.doTriggerMatching.add( inputItem         = ("Analysis::MuonContainer#StacoMuonCollection"),
                                          matchToItemList   = [ triggerName ],
                                          matchDistanceList = [ 0.15 ] )
    AODSelectFlags.doTriggerMatching.add( inputItem         = ("Analysis::MuonContainer#Muons"),
                                          matchToItemList   = [ triggerName ],
                                          matchDistanceList = [ 0.15 ] )
    AODSelectFlags.doTriggerMatching.add( inputItem         = "ElectronContainer#ElectronAODCollection",
                                          matchToItemList   = [ triggerName ],
                                          matchDistanceList = [ 0.15 ] )
    pass





#====================================================================
# Do the PrimaryVertex selection
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "VertexSelectorInH4lBremRecoStream",
                                   OutputLevel          = INFO,
                                   inputCollection      = 'VxPrimaryCandidate',
                                   minNumberVxTracks    = 3
                                   )
h4lAlgsToBookkeep.append("VertexSelectorInH4lBremRecoStream")





#====================================================================
# Define the muon selections
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
topSequence += D2PDMuonSelector( "LowEtStacoMuonSelectorInH4lBremRecoStream",
                                 OutputLevel                    = INFO,
                                 inputCollection                = 'StacoMuonCollection',
                                 outputLinkCollection           = 'LowEtStacoMuonLinkCollection',
                                 minNumberPassed                = 2,
                                 muonRequireIsLoose             = True,
                                 etMin                          = 4.0*Units.GeV,
                                 #absEtaMax                      = 2.8,
                                 associationToolList            = h4lMuAssociationToolList,
                                 outputAssociationContainerList = h4lMuOutputAssociationContainerList
                                 )
h4lAlgsToBookkeep.append("LowEtStacoMuonSelectorInH4lBremRecoStream")

topSequence += D2PDMuonSelector( "LowEtMuonSelectorInH4lBremRecoStream",
                                 OutputLevel                    = INFO,
                                 inputCollection                = 'Muons',
                                 outputLinkCollection           = 'LowEtMuonLinkCollection',
                                 minNumberPassed                = 2,
                                 muonRequireIsLoose             = True,
                                 etMin                          = 4.0*Units.GeV,
                                 #absEtaMax                      = 2.8,
                                 associationToolList            = h4lMuAssociationToolList,
                                 outputAssociationContainerList = h4lMuOutputAssociationContainerList
                                 )
h4lAlgsToBookkeep.append("LowEtMuonSelectorInH4lBremRecoStream")

topSequence += D2PDMuonSelector( "CaloMuonSelectorInH4lBremRecoStream",
                                 OutputLevel                    = INFO,
                                 inputCollection                = "CaloMuonCollection",
                                 outputLinkCollection           = 'PreSelectedCaloMuonLinkCollection',
                                 ptMin                          = 12.0*GeV,
                                 absEtaMax                      = 0.12,
                                 minNumberPassed                = 0
                                 )
h4lAlgsToBookkeep.append("CaloMuonSelectorInH4lBremRecoStream")


# Also add the selected calo and staco muons
from D2PDMaker.AddContainers import AddContainers
topSequence += AddContainers( "StacoCaloMuonAdderInH4lAnalysisStream",
                              OutputLevel          = INFO,
                              inputContainerType   = 'INav4MomLinkContainer',
                              inputContainerNames  = [ 'LowEtStacoMuonLinkCollection',
                                                       'PreSelectedCaloMuonLinkCollection' ],
                              outputContainerName  = 'PreSelectedStacoMuonLinkCollection'
                              )
h4lAlgsToBookkeep.append("StacoCaloMuonAdderInH4lAnalysisStream")

topSequence += AddContainers( "MuonsCaloMuonAdderInH4lAnalysisStream",
                              OutputLevel          = INFO,
                              inputContainerType   = 'INav4MomLinkContainer',
                              inputContainerNames  = [ 'LowEtMuonLinkCollection',
                                                       'PreSelectedCaloMuonLinkCollection' ],
                              outputContainerName  = 'PreSelectedMuonLinkCollection'
                              )
h4lAlgsToBookkeep.append("MuonsCaloMuonAdderInH4lAnalysisStream")




#====================================================================
# Define the electron selections
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
topSequence += D2PDElectronSelector( "AllForwardElectronSelectorInH4lBremRecoStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'AllForwardElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronAuthor       = egammaParameters.AuthorFrwd
                                     )
h4lAlgsToBookkeep.append("AllForwardElectronSelectorInH4lBremRecoStream")

topSequence += D2PDElectronSelector( "ForwardElectronSelectorInH4lBremRecoStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'AllForwardElectronLinkCollection',
                                     outputLinkCollection = 'ForwardElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.frwdElectronLoose,
                                     electronAuthor       = egammaParameters.AuthorFrwd,
                                     etMin                = 12.0*Units.GeV
                                     )
h4lAlgsToBookkeep.append("ForwardElectronSelectorInH4lBremRecoStream")



# Check if we have the GSF electron collection available or not
electronColl = 'ElectronAODCollection'
from RecExConfig.ObjKeyStore import objKeyStore
if objKeyStore.isInInput("ElectronContainer", "GSFElectronAODCollection") \
       or objKeyStore['transient'].has_item("ElectronContainer#GSFElectronAODCollection"):
    electronColl = 'GSFElectronAODCollection'
    h4lDPD_msg.info( "Using the GSFElectronAODCollection" )
    pass


# Get the tool to calculate the updated multi-lepton electron identification
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronMultiLeptonSelector
ToolSvc += AthElectronMultiLeptonSelector( "AthElectronMultiLeptonSelectorInH4lBremRecoStream" )

topSequence += D2PDElectronSelector( "LowEtElectronSelectorInH4lBremRecoStream",
                                     OutputLevel                    = INFO,
                                     inputCollection                = electronColl,
                                     outputLinkCollection           = 'LowEtElectronLinkCollection',
                                     minNumberPassed                = 1,
                                     #electronID                     = egammaPID.ElectronIDLoosePP,
                                     electronAuthor                 = egammaParameters.AuthorElectron,
                                     clusterEtMin                   = 4.0*Units.GeV,
                                     #clusterAbsEtaMax               = 2.5,
                                     associationToolList            = h4lElAssociationToolList,
                                     outputAssociationContainerList = h4lElOutputAssociationContainerList
                                     #userDataCalcToolList           = [ ToolSvc.AthElectronMultiLeptonSelectorInH4lBremRecoStream ] # Use AODSelect
                                     )
h4lAlgsToBookkeep.append("LowEtElectronSelectorInH4lBremRecoStream")


# Add the forward electrons with the central electrons
from D2PDMaker.AddContainers import AddContainers
topSequence += AddContainers( "LowEtElectronAdderInH4lBremRecoStream",
                              OutputLevel          = INFO,
                              inputContainerType   = 'INav4MomLinkContainer',
                              inputContainerNames  = [ 'ForwardElectronLinkCollection',
                                                       'LowEtElectronLinkCollection' ],
                              outputContainerName  = 'AllLowEtElectronLinkCollection'
                              )
h4lAlgsToBookkeep.append("LowEtElectronAdderInH4lBremRecoStream")






#====================================================================
# Define the (Z)->e mu selections, i.e., control for ttbar background
#====================================================================
# - the Z->mumu Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "ZmueStacoBosonSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'PreSelectedStacoMuonLinkCollection',
                                                                 'AllLowEtElectronLinkCollection' ],
                                     outputCollection        = 'ZmueStacoBosonCollection',
                                     #chargeMin               = 0,
                                     #chargeMax               = 0,
                                     massMin                 = 30.0*Units.GeV,
                                     #massMax                 = 120.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
algsOR.append("ZmueStacoBosonSelectorInH4lBremRecoStream")

topSequence += D2PDParticleCombiner( "ZmueBosonSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'PreSelectedMuonLinkCollection',
                                                                 'AllLowEtElectronLinkCollection' ],
                                     outputCollection        = 'ZmueBosonCollection',
                                     #chargeMin               = 0,
                                     #chargeMax               = 0,
                                     massMin                 = 30.0*Units.GeV,
                                     #massMax                 = 120.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
algsOR.append("ZmueBosonSelectorInH4lBremRecoStream")





#====================================================================
# Define the Z->mumu selections
#====================================================================
# - the Z->mumu Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "ZmumuStacoBosonOnShellSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'PreSelectedStacoMuonLinkCollection',
                                                                 'PreSelectedStacoMuonLinkCollection' ],
                                     outputCollection        = 'ZmumuStacoBosonOnShellCollection',
                                     #chargeMin               = 0,
                                     #chargeMax               = 0,
                                     massMin                 = 30.0*Units.GeV,
                                     #massMax                 = 120.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
algsOR.append("ZmumuStacoBosonOnShellSelectorInH4lBremRecoStream")

topSequence += D2PDParticleCombiner( "ZmumuBosonOnShellSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'PreSelectedMuonLinkCollection',
                                                                 'PreSelectedMuonLinkCollection' ],
                                     outputCollection        = 'ZmumuBosonOnShellCollection',
                                     #chargeMin               = 0,
                                     #chargeMax               = 0,
                                     massMin                 = 30.0*Units.GeV,
                                     #massMax                 = 120.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
algsOR.append("ZmumuBosonOnShellSelectorInH4lBremRecoStream")

# - the Z->mumu Boson (off-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "ZmumuStacoBosonOffShellSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'PreSelectedStacoMuonLinkCollection',
                                                                 'PreSelectedStacoMuonLinkCollection' ],
                                     outputCollection        = 'ZmumuStacoBosonOffShellCollection',
                                     minNumberPassed         = 1
                                     )
h4lAlgsToBookkeep.append("ZmumuStacoBosonOffShellSelectorInH4lBremRecoStream")

topSequence += D2PDParticleCombiner( "ZmumuBosonOffShellSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'PreSelectedMuonLinkCollection',
                                                                 'PreSelectedMuonLinkCollection' ],
                                     outputCollection        = 'ZmumuBosonOffShellCollection',
                                     minNumberPassed         = 1
                                     )
h4lAlgsToBookkeep.append("ZmumuBosonOffShellSelectorInH4lBremRecoStream")



# ====================================================================
# Define the Z->ee selections
# ====================================================================
# - the Z->ee Boson (on-shell)
topSequence += D2PDParticleCombiner( "ZeeBosonOnShellSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'LowEtElectronLinkCollection',
                                                                 'AllLowEtElectronLinkCollection' ],
                                     outputCollection        = 'ZeeBosonOnShellCollection',
                                     #chargeMin               = -1,
                                     #chargeMax               = 1,
                                     massMin                 = 30.0*Units.GeV,
                                     #massMax                 = 120.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
algsOR.append("ZeeBosonOnShellSelectorInH4lBremRecoStream")

# - the Z->ee Boson (off-shell)
topSequence += D2PDParticleCombiner( "ZeeBosonOffShellSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'LowEtElectronLinkCollection',
                                                                 'AllLowEtElectronLinkCollection' ],
                                     outputCollection        = 'ZeeBosonOffShellCollection',
                                     minNumberPassed         = 1
                                     )
h4lAlgsToBookkeep.append("ZeeBosonOffShellSelectorInH4lBremRecoStream")


# ====================================================================
# Define the Higgs selections
# ====================================================================
# H->4e
topSequence += D2PDParticleCombiner( "H4eSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZeeBosonOffShellCollection',
                                                                 'ZeeBosonOffShellCollection' ],
                                     outputCollection        = 'H4eCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H4eSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H4eSelectorInH4lBremRecoStream")

# H->2e2mu
topSequence += D2PDParticleCombiner( "H2e2muSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZeeBosonOffShellCollection',
                                                                 'ZmumuBosonOffShellCollection' ],
                                     outputCollection        = 'H2e2muCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H2e2muSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H2e2muSelectorInH4lBremRecoStream")

topSequence += D2PDParticleCombiner( "H2e2muStacoSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZeeBosonOffShellCollection',
                                                                 'ZmumuStacoBosonOffShellCollection' ],
                                     outputCollection        = 'H2e2muStacoCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H2e2muStacoSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H2e2muStacoSelectorInH4lBremRecoStream")

# H->2mu2e
topSequence += D2PDParticleCombiner( "H2mu2eSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZmumuBosonOffShellCollection',
                                                                 'ZeeBosonOffShellCollection' ],
                                     outputCollection        = 'H2mu2eCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H2mu2eSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H2mu2eSelectorInH4lBremRecoStream")

topSequence += D2PDParticleCombiner( "H2mu2eStacoSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZmumuStacoBosonOffShellCollection',
                                                                 'ZeeBosonOffShellCollection' ],
                                     outputCollection        = 'H2mu2eStacoCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H2mu2eStacoSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H2mu2eStacoSelectorInH4lBremRecoStream")


# H->4mu
topSequence += D2PDParticleCombiner( "H4muSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZmumuBosonOffShellCollection',
                                                                 'ZmumuBosonOffShellCollection' ],
                                     outputCollection        = 'H4muCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H4muSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H4muSelectorInH4lBremRecoStream")

topSequence += D2PDParticleCombiner( "H4muStacoSelectorInH4lBremRecoStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                     inputCollectionList     = [ 'ZmumuStacoBosonOffShellCollection',
                                                                 'ZmumuStacoBosonOffShellCollection' ],
                                     outputCollection        = 'H4muStacoCollection',
                                     #massMin                 = 50.0*Units.GeV,
                                     minNumberPassed         = 1
                                     )
#algsOR.append("H4muStacoSelectorInH4lBremRecoStream")
h4lAlgsToBookkeep.append("H4muStacoSelectorInH4lBremRecoStream")




# Create a list of high-pt jets. We will use those to thin the CaloClusters
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
topSequence += D2PDJetSelector( "HighEtAntiKt4TopoEMJetsSelectorInH4lBremRecoStream",
                                OutputLevel          = INFO,
                                inputCollection      = "VeryLooseCleanLinkAntiKt4TopoEMJets",
                                outputCollection     = "HighEtVeryLooseCleanAntiKt4TopoEMJets",
                                outputLinkCollection = "HighEtVeryLooseCleanAntiKt4TopoEMJetLinks",
                                minNumberPassed      = 0,
                                ptMin                = 20.0*Units.GeV
                                )
h4lAlgsToBookkeep.append("HighEtAntiKt4TopoEMJetsSelectorInH4lBremRecoStream")

topSequence += D2PDJetSelector( "HighEtAntiKt4LCTopoJetsSelectorInH4lBremRecoStream",
                                OutputLevel          = INFO,
                                inputCollection      = "VeryLooseCleanLinkAntiKt4LCTopoJets",
                                outputCollection     = "HighEtVeryLooseCleanAntiKt4LCTopoJets",
                                outputLinkCollection = "HighEtVeryLooseCleanAntiKt4LCTopoJetLinks",
                                minNumberPassed      = 0,
                                ptMin                = 20.0*Units.GeV
                                )
h4lAlgsToBookkeep.append("HighEtAntiKt4LCTopoJetsSelectorInH4lBremRecoStream")








# Add the re-calculation of the isolation and creation of the maps here

#include ( "TrackInCaloTools/TrackInCaloTools_jobOptions.py" )
#ToolSvc.TrackInCaloTools.CaloCellContainer = CalorimeterCellContainer
#import egammaRec.EMCommonTrackSummary

# Get the track isolation calculation tools
from egammaRec.EMTrackIsolationTool import egammaTrackIsolationTool
ToolSvc += egammaTrackIsolationTool("EMTrackIsolationToolInH4lStream")
if jobproperties.egammaRecFlags.useBremTracks==True:
    ToolSvc.EMTrackIsolationToolInH4lStream.useBremAssoc = True
    pass
ToolSvc.EMTrackIsolationToolInH4lStream.OutputLevel = INFO

# Change the calo cell container based on the input
CalorimeterCellContainer = "AllCalo"
from RecExConfig.RecFlags import rec 
if rec.readAOD() : 
    CalorimeterCellContainer = "AODCellContainer"
    pass
ToolSvc.EMTrackIsolationElectronTrackIsolationTool.CaloCellContainer           = CalorimeterCellContainer
ToolSvc.EMTrackIsolationElectronTrackIsolationTool.OutputLevel                 = INFO
ToolSvc.EMTrackIsolationElectronTrackIsolationTool.InputObjectContainer        = "egClusterCollection"      # m_objectContainerKey This should be the egClusterContainer
ToolSvc.EMTrackIsolationElectronTrackIsolationTool.InputTrackParticleContainer = "GSFTrackParticleCandidate"   # m_inTPContainerKey
ToolSvc.EMTrackIsolationElectronTrackIsolationTool.OutputTrackParticleAssocs   = "GSFTrackParticleCandidate_ElectronIsoMatchTo_TrackParticleCandidate"  # m_partToTrackPartAssocs
ToolSvc.EMTrackIsolationElectronTrackIsolationTool.OutputINav4MomToTrackParicleAssocs = "egClusterCollection_ElectronIsoMatchTo_TrackParticleCandidate"  # m_partToTrackPartAssocs

ToolSvc.EMTrackIsolationPhotonTrackIsolationTool.CaloCellContainer = CalorimeterCellContainer
ToolSvc.EMTrackIsolationPhotonTrackIsolationTool.OutputLevel = INFO
    



# ====================================================================
# Define the thinning of CaloClusters and TrackParticles 
# ====================================================================
from AthenaCommon.AlgSequence import AthSequencer
thinSeqH4lStream = AthSequencer("H4lStreamThinningSeq")
topSequence += thinSeqH4lStream

# Create the combined decission.
# The rest of the thinSeqHighMassDiJetStream will only be executed if this first algorithm accepts the event
from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplySkimming:
    thinSeqH4lStream += PrimaryDPDPrescaler( "StreamH4l_AcceptEvent",
                                             AcceptAlgs = algsOR, # Logical OR of all fitlers
                                             Prescale   = 1
                                             )
    pass



# ====================================================================
# From this point onwards, algorithms are only executed if the current event is accepted
# ====================================================================
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyThinning :


    # Do the calculation of the association maps for the egamma track isolation
    if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyElectronTrackIsolation :
        thinSeqH4lStream += CfgMgr.RunElectronToolsAlg( "RunElectronToolsAlgInH4lStream",
                                                        OutputLevel                        = INFO,
                                                        ElectronContainerName              = "ElectronAODCollection",
                                                        VertexContainerName                = "VxPrimaryCandidate",
                                                        TrackParticleContainerName         = "TrackParticleCandidate",
                                                        OutputTrackParticleAssocs          = "GSFTrackParticleCandidate_ElectronIsoMatchTo_TrackParticleCandidate",
                                                        OutputINav4MomToTrackParicleAssocs = "egClusterCollection_ElectronIsoMatchTo_TrackParticleCandidate",
                                                        ElectronTrackIsolationTool         = ToolSvc.EMTrackIsolationToolInH4lStream
                                                        )
        # Thin TrackParticles that where found in the track isolation cone
        thinSeqH4lStream += ThinTrackParticles( "ThinTrackIsolationConeTrackParticlesInHighMassDiJetStream",
                                                OutputLevel                 = INFO,
                                                thinSvc                     = 'ThinningSvc/H4lStreamThinning',
                                                trackParticleCollection     = 'TrackParticleCandidate',
                                                inputTrackParticleAssocsMap = 'GSFTrackParticleCandidate_ElectronIsoMatchTo_TrackParticleCandidate'
                                                )
    
        if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation and D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyTrackParticleTruthMatching :
            # Merge the matched TrackParticles into one list
            thinSeqH4lStream += CfgMgr.CreateListFromMapsAlg( "CreateListFromMapsAlgInH4lStream",
                                                              OutputLevel                           = INFO,
                                                              InputContainerName                    = "TrackParticleCandidate",
                                                              OutputINav4MomLinkContainerName       = "LinkToMatchedTrackParticleCandidate",
                                                              InputTrackParticleAssocsList          = ["GSFTrackParticleCandidate_ElectronIsoMatchTo_TrackParticleCandidate"],
                                                              InputINav4MomToTrackParicleAssocsList = ["egClusterCollection_ElectronIsoMatchTo_TrackParticleCandidate"]
                                                              )
            # Now, read in this list of TrackParticles and run it through truth matching
            thinSeqH4lStream += CfgMgr.D2PDTrackParticleSelector( "TrackParticleTruthMatchAlgInH4lStream",
                                                                  OutputLevel                    = INFO,
                                                                  inputCollection                = "LinkToMatchedTrackParticleCandidate",
                                                                  associationToolList            = [ ToolSvc.LeptonMCTruthClassifierAssociationToolInH4lBremRecoStream],
                                                                  outputAssociationContainerList = ["TrackParticleCandidate_MatchTo_SpclMC"],
                                                                  forceExecuteAssociation        = True
                                                                  )
            pass
        pass
    


    # Now, try to match each electron to the CaloCalTopoCluster in a given DeltaR cone
    if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyElectronCaloIsolation :
        ToolSvc += CfgMgr.DeltaRAssociationTool( "CaloCalTopoClusterAssociationToolInH4lBremRecoStream",
                                                 OutputLevel                     = INFO,
                                                 inputAssociateToCollection      = "CaloCalTopoCluster",
                                                 deltaRMax                       = 0.4,
                                                 useMatchFromCaloClusterPosition = True,
                                                 userDataPrefix                  = "lep_deltaR04Match_",
                                                 writeUserData                   = False
                                                 )
        thinSeqH4lStream += CfgMgr.D2PDElectronSelector( "MatchElectronsToCaloCalTopoClustersInH4lBremRecoStream",
                                                         OutputLevel                    = INFO,
                                                         inputCollection                = electronColl,
                                                         associationToolList            = [ToolSvc.CaloCalTopoClusterAssociationToolInH4lBremRecoStream],
                                                         outputAssociationContainerList = [electronColl+"_MatchTo_CaloCalTopoCluster"]
                                                         )
        if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation and D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyCaloClusterTruthMatching :
            # Merge the matched TrackParticles into one list
            thinSeqH4lStream += CfgMgr.CreateListFromMapsAlg( "CreateCaloCalTopoClusterListFromMapsAlgInH4lStream",
                                                              OutputLevel                     = INFO,
                                                              InputContainerName              = "CaloCalTopoCluster",
                                                              OutputINav4MomLinkContainerName = "LinkToMatchedCaloCalTopoCluster",
                                                              InputINav4MomAssocsList         = [electronColl+"_MatchTo_CaloCalTopoCluster"]
                                                              )
            # Now, read in this list of TrackParticles and run it through truth matching
            thinSeqH4lStream += CfgMgr.D2PDINavigable4MomentumSelector( "CaloClusterTruthMatchAlgInH4lStream",
                                                                        OutputLevel                    = INFO,
                                                                        inputCollection                = "LinkToMatchedCaloCalTopoCluster",
                                                                        associationToolList            = [ ToolSvc.LeptonMCTruthClassifierAssociationToolInH4lBremRecoStream],
                                                                        outputAssociationContainerList = ["CaloCalTopoCluster_MatchTo_SpclMC"],
                                                                        forceExecuteAssociation        = True
                                                                        )
            pass
        pass



    # Apply the trigger navigation slimming
    allTriggerList = D2PDFlags.WriteDAOD_H4lBremRecStream.ElectronTriggerList + D2PDFlags.WriteDAOD_H4lBremRecStream.MuonTriggerList
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
    from TrigNavTools.TrigNavToolsConfig import HLTStreamTrigNavSlimming
    thinSeqH4lStream += HLTStreamTrigNavSlimming( "StreamH4l_HLTStreamTrigNavSlimming" )
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.OutputLevel         = ERROR
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.ThinningSvc         = 'ThinningSvc/H4lStreamThinning'
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.PrintTree           = 0
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.ProtectOtherStreams = 1
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.Squeeze             = 1
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.RemoveFeatureless   = 1
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.RemoveGhosts        = 1
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.WriteTree           = 1
    thinSeqH4lStream.StreamH4l_HLTStreamTrigNavSlimming.ChainInclusionList  = allTriggerList


    from ThinningUtils.ThinningUtilsConf import ThinTrackParticles
    from ThinningUtils.ThinningUtilsConf import ThinEgammaTrackParticles
    from ThinningUtils.ThinningUtilsConf import ThinCaloClusters
    # Loop over all wanted jet collections:
    # Apply the CaloCalTopoCluster thinning for forward electrons and jets
    thinSeqH4lStream += ThinCaloClusters( "ThinCaloCalTopoClustersInH4lStream",
                                          OutputLevel            = INFO,
                                          thinSvc                = 'ThinningSvc/H4lStreamThinning',
                                          caloClusterCollection  = 'CaloCalTopoCluster',
                                          inputCollectionList    = [ 'AllForwardElectronLinkCollection',
                                                                     'HighEtVeryLooseCleanAntiKt4TopoEMJetLinks',
                                                                     'HighEtVeryLooseCleanAntiKt4LCTopoJetLinks' ]
                                          )
    if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyElectronCaloIsolation :
        thinSeqH4lStream.ThinCaloCalTopoClustersInH4lStream.inputINav4MomAssocsMapList += [ electronColl+"_MatchTo_CaloCalTopoCluster" ]
        pass


    # Apply the TrackParticle thinning for jets, muons
    thinSeqH4lStream += ThinTrackParticles( "ThinTrackParticlesInH4lStream",
                                            OutputLevel             = INFO,
                                            thinSvc                 = 'ThinningSvc/H4lStreamThinning',
                                            trackParticleCollection = 'TrackParticleCandidate',
                                            inputCollectionList     = [ 'HighEtVeryLooseCleanAntiKt4TopoEMJetLinks',
                                                                        'HighEtVeryLooseCleanAntiKt4LCTopoJetLinks',
                                                                        'StacoMuonCollection',
                                                                        'MuidMuonCollection',
                                                                        'Muons',
                                                                        'PreSelectedCaloMuonLinkCollection' ]
                                            )


    # Apply the TrackParticle thinning for egamma
    thinSeqH4lStream += ThinEgammaTrackParticles( "ThinPhotonTrackParticlesInH4lStream",
                                                  OutputLevel                  = INFO,
                                                  thinSvc                      = 'ThinningSvc/H4lStreamThinning',
                                                  trackParticleCollection      = 'TrackParticleCandidate',
                                                  inputCollectionList          = [ 'PhotonAODCollection',
                                                                                   'ElectronAODCollection',
                                                                                   'PhotonAODCollection',
                                                                                   'ElectronAODCollection' ],
                                                  trackParticleAssociationList = [ "GSFTrackAssociation",
                                                                                   "GSFTrackAssociation",
                                                                                   "",
                                                                                   "" ]
                                                  )

    # Apply the GSFTrackParticle thinning for egamma
    thinSeqH4lStream += ThinEgammaTrackParticles( "ThinEGammaGSFTrackParticlesInH4lStream",
                                                  OutputLevel              = INFO,
                                                  thinSvc                  = 'ThinningSvc/H4lStreamThinning',
                                                  trackParticleCollection  = 'GSFTrackParticleCandidate',
                                                  inputCollectionList      = [ 'PhotonAODCollection',
                                                                               'ElectronAODCollection' ]
                                                  )


    # Thin the jets that we don't need any more away
    thinSeqH4lStream += CfgMgr.ThinINavigable4MomentumCollection( "ThinAntiKt4TopoEMJetsInH4lStream",
                                                                  OutputLevel                    = INFO,
                                                                  thinSvc                        = 'ThinningSvc/H4lStreamThinning',
                                                                  collectionToThin               = "AntiKt4TopoEMJets",
                                                                  inputINav4MomLinkContainerList = [ "HighEtVeryLooseCleanAntiKt4TopoEMJetLinks" ]
                                                                  )
    thinSeqH4lStream += CfgMgr.ThinINavigable4MomentumCollection( "ThinAntiKt4LCTopoJetsInH4lStream",
                                                                  OutputLevel                    = INFO,
                                                                  thinSvc                        = 'ThinningSvc/H4lStreamThinning',
                                                                  collectionToThin               = "AntiKt4LCTopoJets",
                                                                  inputINav4MomLinkContainerList = [ "HighEtVeryLooseCleanAntiKt4LCTopoJetLinks" ]
                                                                  )


    pass # End: if ApplyThinning


    



# ====================================================================
# Reduce the PileUpEventInfo object into the much smaller EventInfo object
# by throwing away all the information about pileup events
# ====================================================================
if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation:
    topSequence += CfgMgr.ReducePileUpEventInfoAlg( "ReducePileUpEventInfoAlgInH4lStream",
                                                    OutputLevel          = INFO,
                                                    ReducedEventInfoName = "H4lEventInfo"
                                                    )
    pass


# ====================================================================
# Define the test DPD output stream
# ====================================================================
H4lBremRecStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplySkimming:
    H4lBremRecStream.AcceptAlgs( algsOR )
    pass

# Bookkeep some more algorithms
H4lBremRecStream.AddOtherAlgsToBookkeep( h4lAlgsToBookkeep )

# ---------------------------------------------------
# Add the containers to the output stream
# ---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
#excludeList = [ "TrigMuonEFContainer#HLT_MuonEF" ]
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


# Deal with only writing out one EventInfo object
H4lBremRecStream.AddItem( ['EventInfo#*'] )
if D2PDFlags.WriteDAOD_H4lBremRecStream.inputIsSimulation:
    H4lBremRecStream.RemoveItem( ['EventInfo#*'] )
    H4lBremRecStream.AddItem( ['EventInfo#H4lEventInfo'] )
    pass

# Just copy a few containers by hand. This way is rather dangerous because
# if you have a typo in the string or if a container name in the AOD changes,
# you will loose this one in the output container!
H4lBremRecStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
H4lBremRecStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
H4lBremRecStream.AddItem( ["egDetailContainer#egDetailAOD"] )
H4lBremRecStream.AddItem( ["egDetailContainer#egDetailContainer"] )
H4lBremRecStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
H4lBremRecStream.AddItem( ["CaloCompactCellContainer#AODCellContainer"] )
H4lBremRecStream.AddItem( ["CaloClusterContainer#egClusterCollection"] )
H4lBremRecStream.AddItem( ["CaloClusterContainer#LArClusterEMSofte"] )
H4lBremRecStream.AddItem( ["CaloClusterContainer#LArClusterEMFrwd"] )
H4lBremRecStream.AddItem( ["CaloClusterContainer#EMTopoSW35"] )
H4lBremRecStream.AddItem( ["CaloCellLinkContainer#egClusterCollection_Link"] )
H4lBremRecStream.AddItem( ["Analysis::MuonContainer#Muons"] )
H4lBremRecStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
H4lBremRecStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
H4lBremRecStream.AddItem( ["Analysis::MuonContainer#CaloMuonCollection"] )
H4lBremRecStream.AddItem( ["CaloClusterContainer#MuonClusterCollection"] )
H4lBremRecStream.AddItem( ["CaloCellLinkContainer#MuonClusterCollection_Link"] )
H4lBremRecStream.AddItem( ["MuonCaloEnergyContainer#MuonCaloEnergyCollection"] )
H4lBremRecStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
#H4lBremRecStream.AddItem( ["JetCollection#HighEtVeryLooseCleanAntiKt4TopoEMJets"] )
#H4lBremRecStream.AddItem( ["JetCollection#HighEtVeryLooseCleanAntiKt4LCTopoJets"] )
#H4lBremRecStream.AddItem( ["JetMomentMap#*"] )
H4lBremRecStream.AddItem( ["JetCollection#AntiKt4TopoEMJets"] )
H4lBremRecStream.AddItem( ["JetMomentMap#AntiKt4TopoEMJetsMomentMap"] )
H4lBremRecStream.AddItem( ["JetCollection#AntiKt4LCTopoJets"] )
H4lBremRecStream.AddItem( ["JetMomentMap#AntiKt4LCTopoJetsMomentMap"] )
#H4lBremRecStream.AddItem( ["JetCollection#AntiKt6TopoEMJets"] )
#H4lBremRecStream.AddItem( ["JetMomentMap#AntiKt6TopoEMJetsMomentMap"] )
#H4lBremRecStream.AddItem( ["JetCollection#AntiKt6LCTopoJets"] )
H4lBremRecStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
H4lBremRecStream.AddItem( ["CaloClusterContainer#CaloCalTopoCluster"] )
H4lBremRecStream.AddItem( ["VxContainer#VxPrimaryCandidate"] )
#H4lBremRecStream.AddItem( ["MissingET#*"] )
H4lBremRecStream.AddItem( ["MissingET#MET_RefFinal"] )
H4lBremRecStream.AddItem( ["MissingET#MET_LocHadTopo"] )
H4lBremRecStream.AddItem( ["MissingET#MET_MuonBoy"] )
H4lBremRecStream.AddItem( ["MissingET#MET_RefMuon_Track"] )
H4lBremRecStream.AddItem( ["LArNoisyROSummary#LArNoisyROSummary"] )
H4lBremRecStream.AddItem( ["SkimDecisionCollection#*"] )

# Add the needed trigger containers
#H4lBremRecStream.AddItem( ["HLT::HLTResult#HLTResult_L2"] )
if D2PDFlags.WriteDAOD_H4lBremRecStream.WriteHLTResult_EF: H4lBremRecStream.AddItem( ["HLT::HLTResult#HLTResult_EF"] )
H4lBremRecStream.AddItem( ["LVL1_ROI#LVL1_ROI"] )
H4lBremRecStream.AddItem( ["TrigDec::TrigDecision#TrigDecision"] )
#H4lBremRecStream.AddItem( ["CTP_Decision#CTP_Decision"] )
## H4lBremRecStream.AddItem( ["TrigRoiDescriptorCollection#HLT"] )
## H4lBremRecStream.AddItem( ["TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma"] )
## H4lBremRecStream.AddItem( ["TrigRoiDescriptorCollection#HLT_forID"] )
## H4lBremRecStream.AddItem( ["TrigRoiDescriptorCollection#HLT_initialRoI"] )
## H4lBremRecStream.AddItem( ["TrigRoiDescriptorCollection#HLT_secondaryRoI_L2"] )
## H4lBremRecStream.AddItem( ["TrigRoiDescriptorCollection#HLT_secondaryRoI_EF"] )
## H4lBremRecStream.AddItem( ["CaloCellContainer#HLT_TrigT2CaloEgammaCells"] )
## H4lBremRecStream.AddItem( ["egammaContainer#HLT_egamma"] )
## H4lBremRecStream.AddItem( ["egammaContainer#HLT_egamma_Electrons"] )
## H4lBremRecStream.AddItem( ["egammaContainer#HLT_egamma_Photons"] )
## H4lBremRecStream.AddItem( ["CaloClusterContainer#HLT"] )
## H4lBremRecStream.AddItem( ["CaloClusterContainer#HLT_TrigCaloClusterMaker"] )
## H4lBremRecStream.AddItem( ["CaloClusterContainer#HLT_TrigCaloClusterMaker_slw"] )
## H4lBremRecStream.AddItem( ["TrigElectronContainer#HLT_L2ElectronFex"] )
## H4lBremRecStream.AddItem( ["TrigElectronContainer#HLT_L2IDCaloFex"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma"] )
## H4lBremRecStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma_robust"] )
## H4lBremRecStream.AddItem( ["egDetailContainer#HLT_egamma"] )
## H4lBremRecStream.AddItem( ["egDetailContainer#HLT_egamma_Electrons"] )
## H4lBremRecStream.AddItem( ["egDetailContainer#HLT_egamma_Photons"] )
## H4lBremRecStream.AddItem( ["TrigPhotonContainer#HLT_L2PhotonFex"] )
## H4lBremRecStream.AddItem( ["TrigEMClusterContainer#HLT"] )
## H4lBremRecStream.AddItem( ["TrigEMClusterContainer#HLT_T2CaloSwCluster"] )
## H4lBremRecStream.AddItem( ["TrigEMClusterContainer#HLT_TrigT2CaloEgamma"] )
## H4lBremRecStream.AddItem( ["TrigEMClusterContainer#HLT_TrigcaloSwCluster"] )
## H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID"] )
## H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID"] )
## H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID"] )
## H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID"] )
## H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID"] )
## H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID"] )

# Add muon TrackParticles
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#SegmentTagTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuTagTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#RefittedExtrapolatedMuonSpectrometerParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#ResolvedForwardTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuTagIMOTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuonboyTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MooreTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuidExtrTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#StatCombinedMuonParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#StacoTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuonSpectrometerParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#ExtrapolatedMuonSpectrometerParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuidCombTrackParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#CombinedFitMuonParticles"] )
H4lBremRecStream.AddItem( ["Rec::TrackParticleContainer#MuGirlRefittedTrackParticles"] )

# Add full Trk::Tracks, if wanted
if D2PDFlags.WriteDAOD_H4lBremRecStream.WriteTrkTracks:
    H4lBremRecStream.AddItem( ['TrackCollection#Tracks'] )
    pass
if D2PDFlags.WriteDAOD_H4lBremRecStream.WriteMuonTrkTracks:
    H4lBremRecStream.AddItem( ['TrackCollection#MuonSlimmedTrackCollection'] )
    pass
if D2PDFlags.WriteDAOD_H4lBremRecStream.WriteGSFTrkTracks:
    H4lBremRecStream.AddItem( ['TrackCollection#GSFTracks'] )
    pass

# Add the Monte Carlo Truth
if D2PDFlags.WriteDAOD_H4lBremRecStream.WriteMCTruth:
    H4lBremRecStream.AddItem( ["TruthParticleContainer#SpclMC"] )
    H4lBremRecStream.AddItem( ["McEventCollection#GEN_AOD"] )
    H4lBremRecStream.AddItem( ["TrackParticleTruthCollection#TrackParticleTruthCollection"] )
    pass


# Add the new GSF containers to the output stream
H4lBremRecStream.AddItem( ['ElectronContainer#GSFElectronAODCollection'] )
H4lBremRecStream.AddItem( ['egDetailContainer#GSFegDetailAOD'] )
H4lBremRecStream.AddItem( ['Rec::TrackParticleContainer#GSFTrackParticleCandidate'] )
#H4lBremRecStream.AddItem( ['TrackCollection#GSFTracks'] )
H4lBremRecStream.AddItem( ['CaloClusterContainer#GSFElectronsClusters'] )
H4lBremRecStream.AddItem( ['CaloCellLinkContainer#GSFElectronsClusters_Link'] )
H4lBremRecStream.AddItem( ['INav4MomAssocs#GSFElectronPhotonAssociation'] )
H4lBremRecStream.AddItem( ['INav4MomAssocs#GSFElectronsAssociation'] )
H4lBremRecStream.AddItem( ['TrackParticleAssocs#GSFTrackAssociation'] )
H4lBremRecStream.AddItem( ['TrackParticleTruthCollection#GSFTrackParticleTruthCollection'] )

H4lBremRecStream.AddItem( ['INav4MomAssocs#*'] )
H4lBremRecStream.AddItem( ['INav4MomToTrackParticleAssocs#*'] )
H4lBremRecStream.AddItem( ['TrackParticleAssocs#*'] )


# You need to add your newly created output containers from above to the output stream
H4lBremRecStream.AddItem( ['CompositeParticleContainer#*'] )
H4lBremRecStream.AddItem( ['INav4MomLinkContainer#*'] )
H4lBremRecStream.AddItem( ['INav4MomAssocs#*'] )

# Also write out the user data
H4lBremRecStream.AddItem( ['UserDataStore#*'] )



#====================================================================
# UserDataSvc
#====================================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'UserDataSvc'):
    from AthenaCommon import CfgMgr
    svcMgr += CfgMgr.UserDataSvc()
    pass
#svcMgr.UserDataSvc.CreateStoreAtBegin = True


#====================================================================
# Define the ThinningSvc
#====================================================================
if D2PDFlags.WriteDAOD_H4lBremRecStream.ApplyThinning :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    augStream = MSMgr.GetStream( streamName )
    evtStream = augStream.GetEventStream()
    svcMgr += createThinningSvc( svcName="H4lStreamThinning", outStreams=[evtStream] )
    pass


