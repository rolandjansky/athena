##-----------------------------------------------------------------------------
## Name: D2PD_ExampleSimpleZtautau.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              Z -> tau tau output stream.
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
from D2PDMaker.D2PDMakerConf import D2PDTauSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDTauSelector( "TauSelectorInExampleSimpleZtautauStream",
                           OutputLevel              = INFO,
                           inputCollection          = 'TauJetContainer',
                           outputLinkCollection     = 'MyZtautauLooseTauLinkCollection',
                           minNumberPassed          = 1,
                           #tauIsTau                 = TauJetParameters.TauCutSafeMedium,
                           #tauAuthor                = TauJetParameters.tauRec,
                           etMin                    = 15.0*Units.GeV,
                           etaMin                   = -2.5,
                           etaMax                   = 2.5,
                           etaVetoRanges            = "[-1.52, -1.37], [1.37, 1.52]",
                           acceptChargeList         = [ 1 ],
                           allowChargeConjugate     = True,
                           acceptNumberOfTracksList = [ 1, 3 ]
                           )



#====================================================================
# Define the particle combinations 
#====================================================================
# - the Z->ee Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleSimpleZtautauStream_ZtautauBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 23, # This is a Z Boson
                                inputCollectionList     = [ 'MyZtautauLooseTauLinkCollection', 'MyZtautauLooseTauLinkCollection' ],
                                outputCollection        = 'MyZtautauLooseZtautauBosonCollection',
                                minNumberPassed         = 1,
                                chargeMax               = 0,
                                massMin                 = 70.0*Units.GeV,
                                massMax                 = 110.0*Units.GeV
                                )



#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AODM_MyZtautau"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDExampleSimpleZtautauStream.pool.root"
ExampleSimpleZtautauStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ExampleSimpleZtautauStream.AcceptAlgs( ["TauSelectorInExampleSimpleZtautauStream",
                                        "D2PDParticleCombinerInExampleSimpleZtautauStream_ZtautauBoson"] )



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
ExampleSimpleZtautauStream.AddItem( ['EventInfo#*'] )
ExampleSimpleZtautauStream.AddItem( ["egammaContainer#TauAODCollection"] )
ExampleSimpleZtautauStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
ExampleSimpleZtautauStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ExampleSimpleZtautauStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ExampleSimpleZtautauStream.AddItem( ["egDetailContainer#egDetailContainer"] )
ExampleSimpleZtautauStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
ExampleSimpleZtautauStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
ExampleSimpleZtautauStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
ExampleSimpleZtautauStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
ExampleSimpleZtautauStream.AddItem( ["JetCollection#Cone4TowerJets"] )
ExampleSimpleZtautauStream.AddItem( ["JetCollection#Cone4TopoJets"] )
ExampleSimpleZtautauStream.AddItem( ["JetCollection#Cone7TowerJets"] )
ExampleSimpleZtautauStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
ExampleSimpleZtautauStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )

# You need to add your newly created output containers from above to the output stream
ExampleSimpleZtautauStream.AddItem( ['CompositeParticleContainer#MyZtautauLooseZtautauBosonCollection'] )
ExampleSimpleZtautauStream.AddItem( ['INav4MomLinkContainer#MyZtautauLooseTauLinkCollection'] )




#====================================================================
# UserDataSvc, only really needed/used when UserData is computed...
#====================================================================
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataInExampleSimpleZtautauStream")
svcMgr.UserDataInExampleSimpleZtautauStream.OutputStream = ExampleSimpleZtautauStream.Stream
