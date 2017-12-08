##-----------------------------------------------------------------------------
## Name: D2PD_Test.py
##
## Author: Elias Coniavitis (Uppsala), based on code by Karsten Koeneke (DESY)
## Email:  elias.coniavitis@cern.ch; karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              for HSG5
##
##-----------------------------------------------------------------------------

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 



#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
streamName = D2PDFlags.WriteExampleSimpleTestStream.StreamName
fileName   = buildFileName( D2PDFlags.WriteExampleSimpleTestStream )
ExampleTestStream = MSMgr.NewPoolStream( streamName, fileName )


ExampleTestStream.AddItem( 'TruthParticleContainer#SpclMC' )
ExampleTestStream.AddItem( 'MissingEtTruth#MET_Truth_PileUp' )
ExampleTestStream.AddItem( 'MissingET#MET_RefFinal' )
ExampleTestStream.AddItem( 'TrigDec::TrigDecision#TrigDecision' )
ExampleTestStream.AddItem( 'MissingEtTruth#MET_Truth' )
ExampleTestStream.AddItem( 'Analysis::MuonContainer#StacoMuonCollection' )
ExampleTestStream.AddItem( 'CTP_Decision#CTP_Decision' )
ExampleTestStream.AddItem( 'EventInfo#*' )
ExampleTestStream.AddItem( 'Analysis::TauJetContainer#TauRecContainer' )
ExampleTestStream.AddItem( 'JetCollection#Cone4TruthJets' )
ExampleTestStream.AddItem( 'JetCollection#AntiKt4TruthJets' )
ExampleTestStream.AddItem( 'JetCollection#Cone4H1TopoJets' )
ExampleTestStream.AddItem( 'JetCollection#AntiKt4H1TopoJets' )
ExampleTestStream.AddItem( 'Analysis::TauDetailsContainer#TauRecDetailsContainer' )
ExampleTestStream.AddItem( 'ElectronContainer#ElectronAODCollection' )
ExampleTestStream.AddItem( 'egDetailContainer#egDetailAOD' )
ExampleTestStream.AddItem( 'JetKeyDescriptor#JetKeyMap' )
ExampleTestStream.AddItem( 'McEventCollection#GEN_AOD' )
ExampleTestStream.AddItem( 'HLT::HLTResult#HLTResult_L2' )
ExampleTestStream.AddItem( 'HLT::HLTResult#HLTResult_EF' )



### Yellow containers

#Trigger stuff
ExampleTestStream.AddItem( 'egammaContainer#HLT_egamma' )
ExampleTestStream.AddItem( 'TrigMuonEFContainer#HLT_MuonEF' )
ExampleTestStream.AddItem( 'TrigMuonEFInfoContainer#HLT_MuonEFInfo' )
#ExampleTestStream.AddItem( 'TrigMissingETContainer#HLT_TrigEFMissingET' )
#ExampleTestStream.AddItem( 'Analysis::TauJetContainer#HLT_TrigTauRecMerged' )
ExampleTestStream.AddItem( 'egDetailContainer#HLT_egamma_Electrons' )
#'Rerunning' stuff
ExampleTestStream.AddItem( 'CaloClusterContainer#CaloCalTopoCluster')
ExampleTestStream.AddItem( 'Rec::TrackParticleContainer#TrackParticleCandidate')



#====================================================================
# UserDataSvc
#====================================================================
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataInExampleTestStream")
svcMgr.UserDataInExampleTestStream.OutputStream = ExampleTestStream.Stream


