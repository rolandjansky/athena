##-----------------------------------------------------------------------------
## Name: D2PD_ExampleSimpleZmumu.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              Z -> mumu output stream.
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
# Define the individual particle selections
#====================================================================
# - the muon selector
# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDMuonSelector( "MuonSelectorInExampleSimpleZmumuStream",
                                 OutputLevel          = INFO,
                                 inputCollection      = 'MuidMuonCollection',
                                 outputLinkCollection = 'MyZmumuTightMuonLinkCollection',
                                 minNumberPassed      = 2,
                                 ptMin                = 15.0*Units.GeV,
                                 muonRequireIsTight   = True,
                                 etaMin               = -2.5,
                                 etaMax               = 2.5
                                 )



#====================================================================
# Define the particle combinations 
#====================================================================
# - the Z->mumu Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleSimpleZmumuStream_ZmumuBoson",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'MyZmumuTightMuonLinkCollection',
                                                                 'MyZmumuTightMuonLinkCollection' ],
                                     outputCollection        = 'MyZmumuLooseZmumuBosonCollection',
                                     minNumberPassed         = 1,
                                     chargeMax               = 0,
                                     massMin                 = 30.0*Units.GeV
                                     )



#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AODM_MyZmumu"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDExampleSimpleZmumuStream.pool.root"
ExampleSimpleZmumuStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ExampleSimpleZmumuStream.AcceptAlgs( ["D2PDParticleCombinerInExampleSimpleZmumuStream_ZmumuBoson"] )



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = []
excludeList = list(set(excludeList)) # This removes dublicates from the list
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

# Just copy a few containers by hand. This way is rather dangerous because
# if you have a typo in the string or if a container name in the AOD changes,
# you will loose this one in the output container!
ExampleSimpleZmumuStream.AddItem( ['EventInfo#*'] )
ExampleSimpleZmumuStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
ExampleSimpleZmumuStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
ExampleSimpleZmumuStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
ExampleSimpleZmumuStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
ExampleSimpleZmumuStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
ExampleSimpleZmumuStream.AddItem( ["JetCollection#AntiKt4TopoEMJets"] )
ExampleSimpleZmumuStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
ExampleSimpleZmumuStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )

# You need to add your newly created output containers from above to the output stream
ExampleSimpleZmumuStream.AddItem( ['CompositeParticleContainer#MyZmumuLooseZmumuBosonCollection'] )
ExampleSimpleZmumuStream.AddItem( ['INav4MomLinkContainer#MyZmumuTightMuonLinkCollection'] )




