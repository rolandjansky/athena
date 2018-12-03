##-----------------------------------------------------------------------------
## Name: D2PD_ExampleSimpleHgamgam.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              H -> gamma gamma output stream.
##
##-----------------------------------------------------------------------------

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

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
# Define the individual particle selections
#====================================================================
# - the electron selector
# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
# Create an instance of this filter, configure it, and add it to the AlgSequence
topSequence += D2PDPhotonSelector( "PhotonSelectorInExampleSimpleHgamgamStream",
                              OutputLevel                      = INFO,
                              inputCollection                  = 'PhotonAODCollection',
                              outputLinkCollection             = 'MyHgamgamLoosePhotonLinkCollection',
                              userDataSvc                      = 'UserDataInExampleSimpleHgamgamStream',
                              minNumberPassed                  = 1,
                              photonIsEM                       = egammaPID.PhotonLoose,
                              photonAcceptAuthorList           = [ egammaParameters.AuthorPhoton, egammaParameters.AuthorRConv ],
                              etMin                            = 0.0*Units.GeV,
                              clusterEtaMin                    = -2.5,
                              clusterEtaMax                    = 2.5,
                              clusterEtaVetoRanges             = "[-1.52, -1.37], [1.37, 1.52]"
                              #egDefaultCaloIsolationDeltaR     = 0.20,
                              #egDefaultAbsoluteCaloEtIsolation = 8.0*Units.GeV
                              )


#====================================================================
# Define the particle combinations 
#====================================================================
# - the Z->ee Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleSimpleHgamgamStream_HBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                inputCollectionList     = [ 'MyHgamgamLoosePhotonLinkCollection', 'MyHgamgamLoosePhotonLinkCollection' ],
                                outputCollection        = 'MyHgamgamLooseHgamgamBosonCollection',
                                minNumberPassed         = 1,
                                massMin                 = 100.0*Units.GeV,
                                massMax                 = 200.0*Units.GeV
                                )



#====================================================================
# Define the trigger selection
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
topSequence += TriggerFilter( "D2PDTriggerFilterInExampleSimpleHgamgamStream_EF_g20",
                         trigger = "EF_g20"
                         )



#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AOD_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AOD_Hgamgam"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDExampleSimpleHgamgamStream.pool.root"
ExampleSimpleHgamgamStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ExampleSimpleHgamgamStream.AcceptAlgs( ["PhotonSelectorInExampleSimpleHgamgamStream",
                                        "D2PDParticleCombinerInExampleSimpleHgamgamStream_HBoson"] )
ExampleSimpleHgamgamStream.RequireAlgs( ["D2PDTriggerFilterInExampleSimpleHgamgamStream_EF_g20"] )


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = []
excludeList = list(set(excludeList)) # This removes dublicates from the list
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


# You need to add your newly created output containers from above to the output stream
ExampleSimpleHgamgamStream.AddItem( ['CompositeParticleContainer#MyHgamgamLooseHgamgamBosonCollection'] )
ExampleSimpleHgamgamStream.AddItem( ['INav4MomLinkContainer#MyHgamgamLoosePhotonLinkCollection'] )




#====================================================================
# UserDataSvc
#====================================================================
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataInExampleSimpleHgamgamStream")
svcMgr.UserDataInExampleSimpleHgamgamStream.OutputStream = ExampleSimpleHgamgamStream.Stream
