##-----------------------------------------------------------------------------
## Name: D2PD_ExampleSimpleZee.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              Z -> ee output stream.
##
##-----------------------------------------------------------------------------

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags
from RecExConfig.RecFlags import rec

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
# Define the individual particle selections
#====================================================================
# - the electron selector
# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDElectronSelector( "ElectronSelectorInExampleSimpleZeeStream",
                                     OutputLevel                      = INFO,
                                     inputCollection                  = 'ElectronAODCollection',
                                     outputLinkCollection             = 'MyZeeLooseElectronLinkCollection',
                                     minNumberPassed                  = 1,
                                     electronIsEM                     = egammaPID.ElectronLoose,
                                     electronAuthor                   = egammaParameters.AuthorElectron,
                                     etMin                            = 15.0*Units.GeV,
                                     clusterEtaMin                    = -2.5,
                                     clusterEtaMax                    = 2.5,
                                     clusterEtaVetoRanges             = "[-1.52, -1.37], [1.37, 1.52]",
                                     )



#====================================================================
# Define the particle combinations 
#====================================================================
# - the Z->ee Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleSimpleZeeStream_ZeeBoson",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'MyZeeLooseElectronLinkCollection', 'MyZeeLooseElectronLinkCollection' ],
                                     outputCollection        = 'MyZeeLooseZeeBosonCollection',
                                     minNumberPassed         = 1,
                                     chargeMax               = 0,
                                     massMin                 = 30.0*Units.GeV,
                                     massMax                 = 200.0*Units.GeV
                                     )



#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AODM_MyZee"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDExampleSimpleZeeStream.pool.root"
ExampleSimpleZeeStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ExampleSimpleZeeStream.AcceptAlgs( ["D2PDParticleCombinerInExampleSimpleZeeStream_ZeeBoson"] )



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
ExampleSimpleZeeStream.AddItem( ['EventInfo#*'] )
ExampleSimpleZeeStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
ExampleSimpleZeeStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
ExampleSimpleZeeStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ExampleSimpleZeeStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ExampleSimpleZeeStream.AddItem( ["egDetailContainer#egDetailContainer"] )
ExampleSimpleZeeStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
ExampleSimpleZeeStream.AddItem( ["CaloClusterContainer#egClusterCollection"] )
ExampleSimpleZeeStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
ExampleSimpleZeeStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
ExampleSimpleZeeStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
ExampleSimpleZeeStream.AddItem( ["JetCollection#AntiKt4TopoEMJets"] )
ExampleSimpleZeeStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
ExampleSimpleZeeStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )

# You need to add your newly created output containers from above to the output stream
ExampleSimpleZeeStream.AddItem( ['CompositeParticleContainer#MyZeeLooseZeeBosonCollection'] )
ExampleSimpleZeeStream.AddItem( ['INav4MomLinkContainer#MyZeeLooseElectronLinkCollection'] )




