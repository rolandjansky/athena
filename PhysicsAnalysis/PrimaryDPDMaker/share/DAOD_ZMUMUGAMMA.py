##---------------------------------------------------
## Name:DAOD_ZMUMUGAMMA.py
##
## Author: Kun Liu(USTC & LPNHE Paris)
## Email:kliu@cern.ch
## 
## Description: This defines the selection of DAOD 
## from containing at least Di-muon + gamma event 

#if this file is already inlcuded, don't include it again
include.block("PrimaryDPDMaker/DAOD_ZMUMUGAMMA.py")

## for messaging
from AthenaCommon.Logging import logging
DAOD_MUStream_msg = logging.getLogger( 'DAOD_MUStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_DiPhotonStream import primDiPhotonDPD
from RecExConfig.RecFlags import rec

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

##============================================================================
## Define the skimming (event selection) for the DIMUON_PHO output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector

# Create the list of filter algorithms to bookkeep
diMuonPhotonBookkeepFilters = []

##====================================================================
## Define the skimming (event selection) for the Di-Muon + Photon DAOD output stream
##====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "DAOD_ZMUMU_VertexFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'VxPrimaryCandidate',
                                       minNumberVxTracks    = 3,
                                       vertexZMin           = -150.0*Units.mm,
                                       vertexZMax           = 150.0*Units.mm,
                                       )
diMuonPhotonBookkeepFilters.append( "DAOD_ZMUMU_VertexFilter" )     
 
# Create a Staco muon filter
topSequence += D2PDMuonSelector( "ZMUMUStream_StacoMuonFilter",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'StacoMuonCollection',
                                     outputLinkCollection = 'ZMUMUStream_StacoMuonLinkCollection',
                                     minNumberPassed      = 1,
                                     ptMin                = 10.0*Units.GeV,
                                     muonRequireIsLoose   = True
                                     )
diMuonPhotonBookkeepFilters.append( "ZMUMUStream_StacoMuonFilter" )

# Create a Calo muon filter
topSequence += D2PDMuonSelector( "ZMUMUStream_CaloMuonFilter",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CaloMuonCollection',
                                     outputLinkCollection = 'ZMUMUStream_CaloMuonLinkCollection',
                                     minNumberPassed      = 1,
                                     ptMin                = 10.0*Units.GeV,
                                     muonRequireIsLoose   = True
                                     )
diMuonPhotonBookkeepFilters.append( "ZMUMUStream_CaloMuonFilter" )

# Create the standard Z -> mu mu filter (Staco)
topSequence += D2PDParticleCombiner( "ZMUMUStream_ZmumuStacoFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'ZMUMUStream_StacoMuonLinkCollection',
                                                                     'ZMUMUStream_StacoMuonLinkCollection' ],
                                         outputCollection        = 'ZMUMUStream_ZmumuStacoBosonCollection',
                                         minNumberPassed         = 1,
                                         massMin                 = 10.0*Units.GeV
                                         )
diMuonPhotonBookkeepFilters.append( "ZMUMUStream_ZmumuStacoFilter" )

topSequence += LogicalFilterCombiner( "DAOD_ZMUMU_ZmumuStacoVertexFilter",
                                          cmdstring = "DAOD_ZMUMU_VertexFilter and ZMUMUStream_ZmumuStacoFilter"
                                          )

# Create the standard Z -> mu mu filter (Calo)
topSequence += D2PDParticleCombiner( "ZMUMUStream_ZmumuCaloFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'ZMUMUStream_CaloMuonLinkCollection',
                                                                     'ZMUMUStream_CaloMuonLinkCollection' ],
                                         outputCollection        = 'ZMUMUStream_ZmumuCaloBosonCollection',
                                         minNumberPassed         = 1,
                                         massMin                 = 10.0*Units.GeV
                                         )
diMuonPhotonBookkeepFilters.append( "ZMUMUStream_ZmumuCaloFilter" )

topSequence += LogicalFilterCombiner( "DAOD_ZMUMU_ZmumuCaloVertexFilter",
                                          cmdstring = "DAOD_ZMUMU_VertexFilter and ZMUMUStream_ZmumuCaloFilter"
                                          )

# Create the standard Z -> mu mu filter (Staco & Calo)
topSequence += D2PDParticleCombiner( "ZMUMUStream_ZmumuStacoCaloFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'ZMUMUStream_StacoMuonLinkCollection',
                                                                     'ZMUMUStream_CaloMuonLinkCollection' ],
                                         outputCollection        = 'ZMUMUStream_ZmumuStacoCaloBosonCollection',
                                         minNumberPassed         = 1,
                                         massMin                 = 10.0*Units.GeV
                                         )
diMuonPhotonBookkeepFilters.append( "ZMUMUStream_ZmumuStacoCaloFilter" )

topSequence += LogicalFilterCombiner( "DAOD_ZMUMU_ZmumuStacoCaloVertexFilter",
                                          cmdstring = "DAOD_ZMUMU_VertexFilter and ZMUMUStream_ZmumuStacoCaloFilter"
                                          )


# Create a photon filter    
topSequence += D2PDPhotonSelector( "DAOD_PHOTONStream_PHOTONFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'PhotonStream_PhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = 10.0*Units.GeV,
                                       clusterEtaMax        = 1000.0,
                                       )

topSequence += LogicalFilterCombiner( "DAOD_ZMUMUGAMMA_ZmumugammaStacoVertexFilter",
                                          cmdstring = "DAOD_ZMUMU_ZmumuStacoVertexFilter and DAOD_PHOTONStream_PHOTONFilter"
                                          )
topSequence += LogicalFilterCombiner( "DAOD_ZMUMUGAMMA_ZmumugammaCaloVertexFilter",
                                          cmdstring = "DAOD_ZMUMU_ZmumuCaloVertexFilter and DAOD_PHOTONStream_PHOTONFilter"
                                          )
topSequence += LogicalFilterCombiner( "DAOD_ZMUMUGAMMA_ZmumugammaStacoCaloVertexFilter",
                                          cmdstring = "DAOD_ZMUMU_ZmumuStacoCaloVertexFilter and DAOD_PHOTONStream_PHOTONFilter"
                                          )

    # Create the combined decission with a possible prescale
#    from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
#    topSequence += PrimaryDPDPrescaler( "DAOD_ZMUMUGAMMA_PrescaleFilter",
#                                        AcceptAlgs = [ "DAOD_ZMUMUGAMMA_ZmumugammaStacoVertexFilter" ],
#                                        Prescale =primDPD.WriteDAOD_MUStream
#                                        )
#    pass

##====================================================================
## Define the DAOD Zmumu output stream
##====================================================================
streamName = primDPD.WriteDAOD_ZMUMUGAMMAStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_ZMUMUGAMMAStream )
DAOD_MUStream = MSMgr.NewPoolStream( streamName, fileName )

DAOD_MUStream.AcceptAlgs( [ "DAOD_ZMUMUGAMMA_ZmumugammaStacoVertexFilter", "DAOD_ZMUMUGAMMA_ZmumugammaCaloVertexFilter" ,
                            "DAOD_ZMUMUGAMMA_ZmumugammaStacoCaloVertexFilter"] )

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
