##-----------------------------------------------------------------------------
## Name: D2PD_HighMDiJet.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the DAODM_ELLOOSE18
##
##-----------------------------------------------------------------------------

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

#---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
fileName   = buildFileName( D2PDFlags.WriteDAODM_HighMDiJetStream )
streamName = D2PDFlags.WriteDAODM_HighMDiJetStream.StreamName




#====================================================================
# Define an empty list for bookkeeping
#====================================================================
algsToBookkeep = []


#====================================================================
# Define the event selection
#====================================================================
# Import the needed filters
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner


#====================================================================
# Check if we have Monte Carlo or real data, based on the inputFileSummary
#====================================================================
inputIsSimulation = False
if inputFileSummary.has_key("evt3_type"):
    eventTypeList = inputFileSummary.get("evt_type")
    if eventTypeList.__contains__("IS_SIMULATION") :
        print "Detected that the input file is a simulated dataset"
        inputIsSimulation = True
        pass
    pass




#====================================================================
# Do the PrimaryVertex selection/flagging
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "VertexSelectorInHighMassDiJetStream",
                                   OutputLevel          = INFO,
                                   inputCollection      = 'VxPrimaryCandidate',
                                   minNumberVxTracks    = 3
                                   )
algsToBookkeep.append( "VertexSelectorInHighMassDiJetStream" )





#====================================================================
# Do the trigger selection/flagging
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for triggerName in D2PDFlags.WriteDAODM_HighMDiJetStream.TriggerList:
    filterName = "TriggerFilterInHighMassDiJetStream_" + triggerName
    topSequence += TriggerFilter( filterName,
                                  trigger = triggerName
                                  )
    algsToBookkeep.append( filterName )
    pass







#====================================================================
# Do the offline selection for photons, electrons and muons
#====================================================================

# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
# Create an instance of this filter, configure it, and add it to the AlgSequence
topSequence += D2PDPhotonSelector( "PhotonTightSelectorInHighMassDiJetStream",
                                   OutputLevel                      = INFO,
                                   inputCollection                  = 'PhotonAODCollection',
                                   outputLinkCollection             = 'PhotonTightLinkCollectionInHighMassDiJetStream',
                                   minNumberPassed                  = 1,
                                   photonIsEM                       = egammaPID.PhotonTight
                                   )

# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDElectronSelector( "ElectronMediumSelectorInHighMassDiJetStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'ElectronMediumLinkCollectionInHighMassDiJetStream',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronMedium,
                                     electronAuthor       = egammaParameters.AuthorElectron
                                     )
algsToBookkeep.append( "ElectronMediumSelectorInHighMassDiJetStream" )

# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDMuonSelector( "StacoMuonTightSelectorInHighMassDiJetStream",
                                 OutputLevel          = INFO,
                                 inputCollection      = 'StacoMuonCollection',
                                 outputLinkCollection = 'StacoMuonTightLinkCollectionInHighMassDiJetStream',
                                 minNumberPassed      = 1,
                                 muonRequireIsTight   = True   
                                 )
topSequence += D2PDMuonSelector( "MuidMuonTightSelectorInHighMassDiJetStream",
                                 OutputLevel          = INFO,
                                 inputCollection      = 'MuidMuonCollection',
                                 outputLinkCollection = 'MuidMuonTightLinkCollectionInHighMassDiJetStream',
                                 minNumberPassed      = 1,
                                 muonRequireIsTight   = True   
                                 )







#====================================================================
# Do the offline selection for all wanted jet collections
#====================================================================
finalFilterNames = []

# Loop over all wanted jet collections:
for jetCollName in D2PDFlags.WriteDAODM_HighMDiJetStream.JetCollections:
    truthJetName       = "AntiKt4TruthJets"
    truthPileupJetName = "AntiKt4TruthPileupJets"
    if jetCollName.__contains__("Kt6") :
        truthJetName       = "AntiKt6TruthJets"
        pass
    
    # --------------------------------------------------------------------
    # If the input is Monte Carlo, also apply truth matching.
    # This will be done for ALL jets
    # and not only for the ones surviving the selection cuts!
    # --------------------------------------------------------------------
    jetAssociationToolList = []
    jetOutputAssociationContainerList = []
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    if inputIsSimulation :
        # Set up a DeltaR association/matching tool
        filterName = jetCollName + "TruthAssociationInHighMassDiJetStream_" + truthJetName
        ToolSvc += DeltaRAssociationTool( filterName,
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = truthJetName,
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Schedule the associations with the electron selector, i.e., performed for all electrons in the ElectronAODCollection
        assert isinstance( filterName, str )
        jetAssociationToolList += [ getattr( ToolSvc, filterName ) ]
        jetOutputAssociationContainerList += [ jetCollName + "_TruthMatch_" + truthJetName ]
        
        filterName = jetCollName + "TruthAssociationInHighMassDiJetStream_" + truthPileupJetName
        ToolSvc += DeltaRAssociationTool( filterName,
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = truthPileupJetName,
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Schedule the associations with the electron selector, i.e., performed for all electrons in the ElectronAODCollection
        assert isinstance( filterName, str )
        jetAssociationToolList += [ getattr( ToolSvc, filterName ) ]
        jetOutputAssociationContainerList += [ jetCollName + "_TruthMatch_" + truthPileupJetName ]
        pass # End: if inputIsSimulation


    # Also, add the trigger associations
    from AssociationComps.AssociationCompsConf import TriggerAssociationTool
    for triggerName in D2PDFlags.WriteDAODM_HighMDiJetStream.TriggerList:
        filterName = jetCollName + "TriggerAssociationInHighMassDiJetStream_" + triggerName
        ToolSvc += TriggerAssociationTool( filterName,
                                           OutputLevel               = INFO,
                                           triggerToMatchTo          = triggerName,
                                           deltaR                    = 0.4,
                                           matchToPassedTriggersOnly = True
                                           )
        assert isinstance( filterName, str )
        jetAssociationToolList += [ getattr( ToolSvc, filterName) ]
        jetOutputAssociationContainerList += [ jetCollName + "_TriggerMatch_" + triggerName ]
        pass
                                    
    
    # Set up the association tools for overlap checks with photons, electrons, muons
    for offlineName in [ "PhotonTight", "ElectronMedium", "StacoMuonTight", "MuidMuonTight" ] :
        filterName = jetCollName + offlineName + "AssociationInHighMassDiJetStream"
        ToolSvc += DeltaRAssociationTool( filterName,
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = offlineName + "LinkCollectionInHighMassDiJetStream",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        assert isinstance( filterName, str )
        jetAssociationToolList += [ getattr( ToolSvc, filterName) ]
        jetOutputAssociationContainerList += [ jetCollName + "_Match_" + offlineName ]
        pass



    from D2PDMaker.D2PDMakerConf import D2PDJetSelector
    # Create an instance of this filter and add it to the AlgSequence
    # Also, configure the filter to your needs. This are just some examples
    filterName = jetCollName + "SelectorInHighMassDiJetStream"
    topSequence += D2PDJetSelector( filterName,
                                    OutputLevel                    = INFO,
                                    inputCollection                = jetCollName,
                                    outputLinkCollection           = jetCollName + 'LinkCollectionInHighMassDiJetStream',
                                    associationToolList            = jetAssociationToolList,
                                    outputAssociationContainerList = jetOutputAssociationContainerList,
                                    minNumberPassed                = 2,
                                    absEtaMax                      = 2.8,
                                    ptMin                          = 30.0*Units.GeV
                                    )
    algsToBookkeep.append( filterName )

    filterName = "HighPt" + jetCollName + "SelectorInHighMassDiJetStream"
    topSequence += D2PDJetSelector( filterName,
                                    OutputLevel          = INFO,
                                    inputCollection      = jetCollName,
                                    outputLinkCollection = 'HighP' + jetCollName + 'LinkCollectionInHighMassDiJetStream',
                                    minNumberPassed      = 1,
                                    absEtaMax            = 2.8,
                                    ptMin                = 280.0*Units.GeV
                                    )
    algsToBookkeep.append( filterName )

    from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
    # - All combinations with pre-selected electrons with 15 GeV
    filterName = "HighMassDi" + jetCollName + "CombinerInHighMassDiJetStream"
    topSequence += D2PDParticleCombiner( filterName,
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'HighP' + jetCollName + 'LinkCollectionInHighMassDiJetStream',
                                                                     jetCollName + 'LinkCollectionInHighMassDiJetStream' ],
                                         outputCollection        = 'HighMassDi' + jetCollName + 'CollectionInHighMassDiJetStream',
                                         minNumberPassed         = 1,
                                         massMin                 = 700.0*Units.GeV
                                         )
    algsToBookkeep.append( filterName )
    finalFilterNames.append( filterName )
    pass





#====================================================================
# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
#====================================================================
from AthenaCommon.AlgSequence import AthSequencer
thinSeqHighMassDiJetStream = AthSequencer("HighMassDiJetStreamThinningSeq")
topSequence += thinSeqHighMassDiJetStream

# Create the combined decission.
# The rest of the thinSeqHighMassDiJetStream will only be executed if this first algorithm accepts the event
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
thinSeqHighMassDiJetStream += PrimaryDPDPrescaler( "StreamHighMassDiJet_AcceptEvent",
                                                   AcceptAlgs = finalFilterNames, # Logical OR of all fitlers
                                                   Prescale   = 1
                                                   )


if D2PDFlags.WriteDAODM_HighMDiJetStream.ApplyThinning :
    # Apply the trigger navigation slimming
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
    from TrigNavTools.TrigNavToolsConfig import HLTStreamTrigNavSlimming
    thinSeqHighMassDiJetStream += HLTStreamTrigNavSlimming( "StreamHighMassDiJet_HLTStreamTrigNavSlimming" )
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.OutputLevel         = INFO
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.ThinningSvc         = 'ThinningSvc/HighMassDiJetStreamThinning'
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.PrintTree           = 0
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.ProtectOtherStreams = 1
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.Squeeze             = 1
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.RemoveFeatureless   = 1
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.RemoveGhosts        = 1
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.WriteTree           = 1
    thinSeqHighMassDiJetStream.StreamHighMassDiJet_HLTStreamTrigNavSlimming.ChainInclusionList  = D2PDFlags.WriteDAODM_HighMDiJetStream.TriggerList

    # Apply thinning
    from ThinningUtils.ThinningUtilsConf import ThinTrackParticles
    from ThinningUtils.ThinningUtilsConf import ThinCaloClusters
    # Loop over all wanted jet collections:
    for jetCollName in D2PDFlags.WriteDAODM_HighMDiJetStream.JetCollections:
        # Apply the CaloCalTopoCluster thinning
        thinSeqHighMassDiJetStream += ThinCaloClusters( "Thin" + jetCollName + "CaloClustersInHighMassDiJetStream",
                                                        OutputLevel           = INFO,
                                                        thinSvc               = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                        caloClusterCollection = 'CaloCalTopoCluster',
                                                        inputCollection       = jetCollName + 'LinkCollectionInHighMassDiJetStream'
                                                        )

        # Apply the TrackParticle thinning
        thinSeqHighMassDiJetStream += ThinTrackParticles( "Thin" + jetCollName + "TrackParticlesInHighMassDiJetStream",
                                                          OutputLevel             = INFO,
                                                          thinSvc                 = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                          trackParticleCollection = 'TrackParticleCandidate',
                                                          inputCollection         = jetCollName + 'LinkCollectionInHighMassDiJetStream'
                                                          )
        pass
    
    # Apply the TrackParticle thinning for other objects
    from ThinningUtils.ThinningUtilsConf import ThinTrackParticles
    thinSeqHighMassDiJetStream += ThinTrackParticles( "ThinPhotonTrackParticlesInHighMassDiJetStream",
                                                      OutputLevel             = INFO,
                                                      thinSvc                 = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                      trackParticleCollection = 'TrackParticleCandidate',
                                                      inputCollection         = 'PhotonAODCollection'
                                                      )
    thinSeqHighMassDiJetStream += ThinTrackParticles( "ThinElectronTrackParticlesInHighMassDiJetStream",
                                                      OutputLevel             = INFO,
                                                      thinSvc                 = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                      trackParticleCollection = 'TrackParticleCandidate',
                                                      inputCollection         = 'ElectronAODCollection'
                                                      )
    thinSeqHighMassDiJetStream += ThinTrackParticles( "ThinStacoMuonTrackParticlesInHighMassDiJetStream",
                                                      OutputLevel             = INFO,
                                                      thinSvc                 = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                      trackParticleCollection = 'TrackParticleCandidate',
                                                      inputCollection         = 'StacoMuonCollection'
                                                      )
    thinSeqHighMassDiJetStream += ThinTrackParticles( "ThinMuidMuonTrackParticlesInHighMassDiJetStream",
                                                      OutputLevel             = INFO,
                                                      thinSvc                 = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                      trackParticleCollection = 'TrackParticleCandidate',
                                                      inputCollection         = 'MuidMuonCollection'
                                                      )
    pass


if D2PDFlags.WriteDAODM_HighMDiJetStream.ApplySlimming :
    from PrimaryDPDMaker.PrimaryDPDMakerConf import SlimTrackInfo
    thinSeqHighMassDiJetStream += SlimTrackInfo( "SlimTrackParticlesInHighMassDiJetStream",
                                                 thinSvc             = 'ThinningSvc/HighMassDiJetStreamThinning',
                                                 TrackPartContName   = 'TrackParticleCandidate'
                                                 )
    pass



#====================================================================
# Define the DAODM output stream
#====================================================================
HighMassDiJetStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
HighMassDiJetStream.AcceptAlgs( [ "StreamHighMassDiJet_AcceptEvent" ] )

HighMassDiJetStream.AddOtherAlgsToBookkeep( algsToBookkeep )

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = [ "TrigMuonEFContainer#HLT_MuonEF" ]
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


# Just copy a few containers by hand. This way is rather dangerous because
# if you have a typo in the string or if a container name in the AOD changes,
# you will loose this one in the output container!
HighMassDiJetStream.AddItem( ['EventInfo#*'] )
HighMassDiJetStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
HighMassDiJetStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
#HighMassDiJetStream.AddItem( ["egDetailContainer#egDetailAOD"] )
#HighMassDiJetStream.AddItem( ["egDetailContainer#egDetailContainer"] )
#HighMassDiJetStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
#HighMassDiJetStream.AddItem( ["CaloCompactCellContainer#AODCellContainer"] )
#HighMassDiJetStream.AddItem( ["CaloClusterContainer#egClusterCollection"] )
#HighMassDiJetStream.AddItem( ["CaloCellLinkContainer#egClusterCollection_Link"] )
HighMassDiJetStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
HighMassDiJetStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
HighMassDiJetStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt4TopoEMJets"] )
HighMassDiJetStream.AddItem( ["JetMomentMap#AntiKt4TopoEMJetsMomentMap"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt6TopoEMJets"] )
HighMassDiJetStream.AddItem( ["JetMomentMap#AntiKt6TopoEMJetsMomentMap"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt4LCTopoJets"] )
HighMassDiJetStream.AddItem( ["JetMomentMap#AntiKt4LCTopoJetsMomentMap"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt6LCTopoJets"] )
HighMassDiJetStream.AddItem( ["JetMomentMap#AntiKt6LCTopoJetsMomentMap"] )
HighMassDiJetStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
HighMassDiJetStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
HighMassDiJetStream.AddItem( ["CaloClusterContainer#CaloCalTopoCluster"] )
HighMassDiJetStream.AddItem( ["VxContainer#VxPrimaryCandidate"] )
#HighMassDiJetStream.AddItem( ["MissingET#*"] )
HighMassDiJetStream.AddItem( ["MissingET#MET_RefFinal"] )
HighMassDiJetStream.AddItem( ["MissingET#MET_LocHadTopo"] )
HighMassDiJetStream.AddItem( ["MissingET#MET_MuonBoy"] )
HighMassDiJetStream.AddItem( ["MissingET#MET_RefMuon_Track"] )
HighMassDiJetStream.AddItem( ["LArNoisyROSummary#LArNoisyROSummary"] )

# Add the needed trigger containers
#HighMassDiJetStream.AddItem( ["HLT::HLTResult#HLTResult_L2"] )
HighMassDiJetStream.AddItem( ["HLT::HLTResult#HLTResult_EF"] )
HighMassDiJetStream.AddItem( ["LVL1_ROI#LVL1_ROI"] )
HighMassDiJetStream.AddItem( ["TrigDec::TrigDecision#TrigDecision"] )
#HighMassDiJetStream.AddItem( ["CTP_Decision#CTP_Decision"] )

# Add the Monte Carlo Truth
HighMassDiJetStream.AddItem( ["TruthParticleContainer#SpclMC"] )
HighMassDiJetStream.AddItem( ["McEventCollection#GEN_AOD"] )
HighMassDiJetStream.AddItem( ["TrackParticleTruthCollection#TrackParticleTruthCollection"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt4TruthJets"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt6TruthJets"] )
HighMassDiJetStream.AddItem( ["JetCollection#AntiKt4TruthPileupJets"] )


# You need to add your newly created output containers from above to the output stream
HighMassDiJetStream.AddItem( ["SkimDecisionCollection#*"] )
HighMassDiJetStream.AddItem( ['CompositeParticleContainer#*'] )
HighMassDiJetStream.AddItem( ['INav4MomLinkContainer#*'] )
HighMassDiJetStream.AddItem( ['INav4MomAssocs#*'] )



#====================================================================
# Define the ThinningSvc
#====================================================================
if D2PDFlags.WriteDAODM_HighMDiJetStream.ApplyThinning or D2PDFlags.WriteDAODM_HighMDiJetStream.ApplySlimming :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    augStream = MSMgr.GetStream( streamName )
    evtStream = augStream.GetEventStream()
    svcMgr += createThinningSvc( svcName="HighMassDiJetStreamThinning", outStreams=[evtStream] )
    pass


