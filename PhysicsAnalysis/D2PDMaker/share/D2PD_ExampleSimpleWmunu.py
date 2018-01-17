##-----------------------------------------------------------------------------
## Name: D2PD_ExampleSimpleWmunu.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              W -> mu nu output stream.
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
# - the electron selector
# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDMuonSelector( "MuonSelectorInExampleSimpleWmunuStream",
                            OutputLevel              = INFO,
                            inputCollection          = 'StacoMuonCollection',
                            outputLinkCollection     = 'MyWmunuLooseMuonLinkCollection',
                            minNumberPassed          = 1,
                            etMin                    = 10.0*Units.GeV,
                            etaMin                   = -2.5,
                            etaMax                   = 2.5,
                            )


#====================================================================
# Define the particle combinations 
#====================================================================
# - the W->mu nu Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDLeptonNeutrinoCombiner
topSequence += D2PDLeptonNeutrinoCombiner( "D2PDLeptonNeutrinoCombinerInExampleSimpleWmunuStream_WmunuBoson",
                                      OutputLevel                  = INFO,
                                      compositeParticlePDG_ID      = 24, # This is a W Boson
                                      inputCollection              = 'MET_RefFinal',
                                      inputLeptonCollection        = 'MyWmunuLooseMuonLinkCollection',
                                      outputNeutrinoCollection     = 'MyWmunuNeutrinoCollection',
                                      outputNeutrinoLinkCollection = 'MyWmunuNeutrinoLinkCollection',
                                      outputCollection             = 'MyWmunuLooseWmunuBosonCollection',
                                      minNumberPassed              = 1,
                                      missingEtMin                 = 20.0*Units.GeV,
                                      massMin                      = 64.0*Units.GeV,
                                      massMax                      = 104.0*Units.GeV
                                      )




#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AODM_Wmunu"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDExampleSimpleWmunuStream.pool.root"
ExampleSimpleWmunuStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ExampleSimpleWmunuStream.AcceptAlgs( ["MuonSelectorInExampleSimpleWmunuStream",
                                      "D2PDLeptonNeutrinoCombinerInExampleSimpleWmunuStream_WmunuBoson"] )



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
ExampleSimpleWmunuStream.AddItem( ['EventInfo#*'] )
ExampleSimpleWmunuStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
ExampleSimpleWmunuStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
ExampleSimpleWmunuStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ExampleSimpleWmunuStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ExampleSimpleWmunuStream.AddItem( ["egDetailContainer#egDetailContainer"] )
ExampleSimpleWmunuStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
ExampleSimpleWmunuStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
ExampleSimpleWmunuStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
ExampleSimpleWmunuStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
ExampleSimpleWmunuStream.AddItem( ["JetCollection#Cone4TowerJets"] )
ExampleSimpleWmunuStream.AddItem( ["JetCollection#Cone4TopoJets"] )
ExampleSimpleWmunuStream.AddItem( ["JetCollection#Cone7TowerJets"] )
ExampleSimpleWmunuStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )


# You need to add your newly created output containers from above to the output stream
ExampleSimpleWmunuStream.AddItem( ['CompositeParticleContainer#MyWmunuLooseWmunuBosonCollection'] )
ExampleSimpleWmunuStream.AddItem( ['NeutrinoContainer#MyWmunuNeutrinoCollection'] )
ExampleSimpleWmunuStream.AddItem( ['INav4MomLinkContainer#MyWmunuLooseMuonLinkCollection'] )
ExampleSimpleWmunuStream.AddItem( ['INav4MomLinkContainer#MyWmunuNeutrinoLinkCollection'] )




