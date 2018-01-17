##-----------------------------------------------------------------------------
## Name: D2PD_Test.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD Test output stream.
##
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
testD2PD_msg = logging.getLogger( 'D2PD_TestStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 




#---- Track Isolation
# This is needed to initialize the TrackIsolationTool properly.
# Without this, there will be ERRORs!
#include( "TrackIsolationTools/TrackIsolationTool_jobOptions.py" )

#---- Muon Isolation
# This is needed to initialize the MuonIsolationTool properly.
# Without this, there will be ERRORs!
#include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

#---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

#---- Load the TauJetParameters
# This is needed to always be up-to-date with the tau
# ID selections and also the author selections
# This only works starting with tauEvent-00-05-21!!!
#cppyy.loadDictionary('tauEventEnumsDict')
#from ROOT import TauJetParameters




# ====================================================================
# Test AODSelect
# ====================================================================
#from AODSelect.AODSelectFlags import AODSelectFlags
#AODSelectFlags.doAll = True

#AODSelectFlags.doLooseCleanJetBuilding.add( inputItem                  = "JetCollection#AntiKt4TopoEMJets",
#                                            createOutputCollection     = True,
#                                            createOutputLinkCollection = True )

#AODSelectFlags.doLooseCleanJetFlagging.add( inputItem = "JetCollection#AntiKt4TopoEMJets" )

#AODSelectFlags.doOfflineMatching.add( inputItem       = "ElectronContainer#ElectronAODCollection",
#                                      matchToItemList = [ "JetCollection#AntiKt4TopoEMJets",
#                                                          "Analysis::MuonContainer#MuidMuonCollection" ] )

#AODSelectFlags.doOfflineMatching.add( inputItem         = "Analysis::MuonContainer#MuidMuonCollection",
#                                      matchToItemList   = [ "ElectronContainer#ElectronAODCollection",
#                                                            "PhotonContainer#PhotonAODCollection" ],
#                                      matchDistanceList = [ 0.2, 0.2 ] )

#AODSelectFlags.doTriggerMatching.add( inputItem         = "ElectronContainer#ElectronAODCollection",
#                                      matchToItemList   = [ "EF_e20_medium", "EF_e20_medium1",
#                                                            "EF_e22_medium", "EF_e22_medium1",
#                                                            "EF_e22vh_medium1", "EF_2e12_medium",
#                                                            "EF_2e12T_medium", "EF_2e12Tvh_medium" ],
#                                      matchDistanceList = [ 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15 ])

#AODSelectFlags.doTruthMatching.add( inputItem = "PhotonContainer#PhotonAODCollection" )
#AODSelectFlags.doTruthMatching.add( inputItem = "ElectronContainer#ElectronAODCollection" )

#AODSelectFlags.print_JobProperties()





# ====================================================================
# Test possible bug
# ====================================================================
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

# Truth-electron selector:
topSequence += D2PDTruthParticleSelector( "TruthLeptonSelectorInH4lBremRecoStream",
                                          OutputLevel             = VERBOSE,
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




#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AODM_Test"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDTestStream.pool.root"
TestStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TestStream.AcceptAlgs( [] )




#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
#from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = []
#excludeList = list(set(excludeList)) # This removes dublicates from the list
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

#TestStream.AddItem( ['ElectronContainer#*'] )
#TestStream.AddItem( ['INav4MomLinkContainer#*'] )
#TestStream.AddItem( ['INav4MomAssocs#*'] )

#TestStream.AddItem( ['Rec::TrackParticleContainer#GSFTrackParticleCandidate'] )
#TestStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
#TestStream.AddItem( ["CaloClusterContainer#CaloCalTopoCluster"] )

TestStream.AddItem( ['PhotonContainer#*'] )
TestStream.AddItem( ['ElectronContainer#*'] )
TestStream.AddItem( ['Analysis::MuonContainer#*'] )
TestStream.AddItem( ['Analysis::TauJetContainer#*'] )
#TestStream.AddItem( ['JetCollection#*'] )
TestStream.AddItem( ['JetCollection#AntiKt4TopoEMJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt4TopoJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt4LCTopoJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt4TowerAODJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt6TowerAODJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt6LCTopoJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt6TopoEMJets'] )
#TestStream.AddItem( ['JetCollection#AntiKt6TopoJets'] )
TestStream.AddItem( ['JetCollection#VeryLoose*'] )
TestStream.AddItem( ['JetCollection#Loose*'] )
TestStream.AddItem( ['JetCollection#Medium*'] )
TestStream.AddItem( ['JetCollection#Tight*'] )
TestStream.AddItem( ['UserDataStore#*'] )
TestStream.AddItem( ['INav4MomLinkContainer#*'] )
TestStream.AddItem( ['INav4MomAssocs#*'] )



#====================================================================
# Define the ThinningSvc
#====================================================================
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="TestStreamThinning", outStreams=[evtStream] )
