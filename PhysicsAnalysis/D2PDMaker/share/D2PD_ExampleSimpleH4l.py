##-----------------------------------------------------------------------------
## Name: D2PD_Test.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              H -> Z Z* -> 4l
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
# - the muon selector --- for testing!
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
topSequence += D2PDMuonSelector( "MuonSelectorInExampleH4lStream",
                            OutputLevel              = INFO,
                            inputCollection          = 'StacoMuonCollection',
                            outputLinkCollection     = 'MyH4lLooseMuonLinkCollection',
                            minNumberPassed          = 2,
                            etMin                    = 5.0*Units.GeV,
                            etaMin                   = -2.5,
                            etaMax                   = 2.5,
                            etaVetoRanges            = "[-1.52, -1.37], [1.37, 1.52]"
                            )

# - the electron selector
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
topSequence += D2PDElectronSelector( "ElectronSelectorInExampleH4lStream",
                                OutputLevel                  = INFO,
                                inputCollection              = 'ElectronAODCollection',
                                outputLinkCollection         = 'MyH4lLooseElectronLinkCollection',
                                minNumberPassed              = 2,
                                etMin                        = 15.0*Units.GeV,
                                etaMin                       = -2.5,
                                etaMax                       = 2.5,
                                etaVetoRanges                = "[-1.52, -1.37], [1.37, 1.52]",
                                electronIsEM                 = egammaPID.ElectronLoose,
                                electronAuthor               = egammaParameters.AuthorElectron
                                )



#====================================================================
# Define the particle combinations 
#====================================================================

# - the Z->mumu Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_ZmumuBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 23, # This is a Z Boson
                                inputCollectionList     = [ 'MyH4lLooseMuonLinkCollection', 'MyH4lLooseMuonLinkCollection' ],
                                outputCollection        = 'MyH4lLooseZmumuBosonCollection',
                                minNumberPassed         = 1,
                                chargeMax               = 0,
                                massMin                 = 70.0*Units.GeV,
                                massMax                 = 110.0*Units.GeV
                                )

# - the Z->mumu Boson (off-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_ZmumuBosonOffShell",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 23, # This is a Z Boson
                                inputCollectionList     = [ 'MyH4lLooseMuonLinkCollection', 'MyH4lLooseMuonLinkCollection' ],
                                outputCollection        = 'MyH4lLooseZmumuBosonOffShellCollection',
                                minNumberPassed         = 1,
                                chargeMax               = 0,
                                massMin                 = 20.0*Units.GeV,
                                massMax                 = 110.0*Units.GeV
                                )

# - the Z->ee Boson (on-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_ZeeBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 23, # This is a Z Boson
                                inputCollectionList     = [ 'MyH4lLooseElectronLinkCollection', 'MyH4lLooseElectronLinkCollection' ],
                                outputCollection        = 'MyH4lLooseZeeBosonCollection',
                                minNumberPassed         = 1,
                                chargeMax               = 0,
                                massMin                 = 70.0*Units.GeV,
                                massMax                 = 110.0*Units.GeV
                                )

# - the Z->ee Boson (off-shell)
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_ZeeBosonOffShell",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 23, # This is a Z Boson
                                inputCollectionList     = [ 'MyH4lLooseElectronLinkCollection', 'MyH4lLooseElectronLinkCollection' ],
                                outputCollection        = 'MyH4lLooseZeeBosonOffShellCollection',
                                minNumberPassed         = 1,
                                chargeMax               = 0,
                                massMin                 = 20.0*Units.GeV,
                                massMax                 = 120.0*Units.GeV
                                )


# - the Higgs->4e Boson
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_Higgs4eBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                inputCollectionList     = [ 'MyH4lLooseZeeBosonCollection', 'MyH4lLooseZeeBosonOffShellCollection' ],
                                outputCollection        = 'MyH4lLooseHiggs4eBosonCollection',
                                minNumberPassed         = 1,
                                massMin                 = 100.0*Units.GeV
                                )

# - the Higgs->2mu2e Boson
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_Higgs2mu2eBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                inputCollectionList     = [ 'MyH4lLooseZmumuBosonCollection', 'MyH4lLooseZeeBosonOffShellCollection' ],
                                outputCollection        = 'MyH4lLooseHiggs2mu2eBosonCollection',
                                minNumberPassed         = 1,
                                massMin                 = 100.0*Units.GeV
                                )


# - the Higgs->2e2mu Boson
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_Higgs2e2muBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                inputCollectionList     = [ 'MyH4lLooseZeeBosonCollection', 'MyH4lLooseZmumuBosonOffShellCollection' ],
                                outputCollection        = 'MyH4lLooseHiggs2e2muBosonCollection',
                                minNumberPassed         = 1,
                                massMin                 = 100.0*Units.GeV
                                )


# - the Higgs->4mu Boson
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
topSequence += D2PDParticleCombiner( "D2PDParticleCombinerInExampleH4lStream_Higgs4muBoson",
                                OutputLevel             = INFO,
                                compositeParticlePDG_ID = 25, # This is a Higgs Boson
                                inputCollectionList     = [ 'MyH4lLooseZmumuBosonCollection', 'MyH4lLooseZmumuBosonOffShellCollection' ],
                                outputCollection        = 'MyH4lLooseHiggs4muBosonCollection',
                                minNumberPassed         = 1,
                                massMin                 = 100.0*Units.GeV
                                )





#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AOD_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
streamName = "StreamD2AOD_H4l"
fileName   = D2PDFlags.OutputDirectoryName() + "MyD2PDExampleSimpleH4lStream.pool.root"
ExampleH4lStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ExampleH4lStream.AcceptAlgs( ["ElectronSelectorInExampleH4lStream",
                              "MuonSelectorInExampleH4lStream",
                              "D2PDParticleCombinerInExampleH4lStream_ZmumuBoson",
                              "D2PDParticleCombinerInExampleH4lStream_ZmumuBosonOffShell",
                              "D2PDParticleCombinerInExampleH4lStream_ZeeBoson",
                              "D2PDParticleCombinerInExampleH4lStream_ZeeBosonOffShell",
                              "D2PDParticleCombinerInExampleH4lStream_Higgs4eBoson",
                              "D2PDParticleCombinerInExampleH4lStream_Higgs2mu2eBoson",
                              "D2PDParticleCombinerInExampleH4lStream_Higgs2e2muBoson",
                              "D2PDParticleCombinerInExampleH4lStream_Higgs4muBoson"] )



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
excludeList = []
excludeList = list(set(excludeList)) # This removes dublicates from the list
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


ExampleH4lStream.AddItem( ['INav4MomLinkContainer#MyH4lLooseMuonLinkCollection'] )
ExampleH4lStream.AddItem( ['INav4MomLinkContainer#MyH4lLooseElectronLinkCollection'] )

ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseZmumuBosonCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseZmumuBosonOffShellCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseZeeBosonCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseZeeBosonOffShellCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseHiggs4eBosonCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseHiggs2mu2eBosonCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseHiggs2e2muBosonCollection'] )
ExampleH4lStream.AddItem( ['CompositeParticleContainer#MyH4lLooseHiggs4muBosonCollection'] )




#====================================================================
# UserDataSvc
#====================================================================
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataInExampleH4lStream")
svcMgr.UserDataInExampleH4lStream.OutputStream = ExampleH4lStream.Stream
