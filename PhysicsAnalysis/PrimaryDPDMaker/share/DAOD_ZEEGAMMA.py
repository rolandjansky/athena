##---------------------------------------------------
## Name:DAOD_ZEEGAMMA.py
##
## Author: Kun Liu(USTC & LPNHE Paris)
## Email:kliu@cern.ch
## 
## Description: This defines the selection of DAOD 
## from containing at least Di-electron + gamma event 

#if this file is already inlcuded, don't include it again
include.block("PrimaryDPDMaker/DAOD_ZEEGAMMA.py")

## for messaging
from AthenaCommon.Logging import logging
DAOD_EGammaStream_msg = logging.getLogger( 'DAOD_EGammaStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
    
## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

##============================================================================
## Define the skimming (event selection) for the DAOD_ZEEPHOTON output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner

# Create the list of filter algorithms to bookkeep
diElectronPHOBookkeepFilters = []

##====================================================================
## Define the skimming (event selection) for the ZEEPHO DAOD output stream
##====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "DAOD_ZEE_VertexFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'VxPrimaryCandidate',
                                       minNumberVxTracks    = 3,
                                       vertexZMin           = -150.0*Units.mm,
                                       vertexZMax           = 150.0*Units.mm,
                                       )
diElectronPHOBookkeepFilters.append( "DAOD_ZEE_VertexFilter" )

# Loose electron filter
topSequence += D2PDElectronSelector( "DAOD_ZEE_LooseElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputCollection       = 'DAOD_ZEE_LooseElectronCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = [ egammaParameters.AuthorFrwd ],
                                         electronID             = egammaPID.ElectronIDLoosePP,
                                         clusterEtMin           = 10.0*Units.GeV
                                         )
diElectronPHOBookkeepFilters.append( "DAOD_ZEE_LooseElectronFilter" )

# Define the Z -> e e filter
topSequence += D2PDParticleCombiner( "DAOD_ZEEStream_ZeeFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'DAOD_ZEE_LooseElectronCollection',
                                                                     'DAOD_ZEE_LooseElectronCollection' ],
                                         outputCollection        = 'DAOD_ZEEStream_ZeeBosonCollection',
                                         minNumberPassed         = 1,
                                         massMin                 = 10.0*Units.GeV
                                         )
diElectronPHOBookkeepFilters.append( "DAOD_ZEEStream_ZeeFilter" )

topSequence += LogicalFilterCombiner( "DAOD_ZEEStream_ZeeVertexFilter",
                                      cmdstring = "DAOD_ZEEStream_ZeeFilter and DAOD_ZEE_VertexFilter" )
diElectronPHOBookkeepFilters.append( "DAOD_ZEEStream_ZeeVertexFilter" )
# Photon filter   
topSequence += D2PDPhotonSelector( "DAOD_PHOTONStream_PHOTONFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'PhotonStream_PhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = 10.0*Units.GeV,
                                       clusterEtaMax        = 1000.0,
                                       )

diElectronPHOBookkeepFilters.append( "DAOD_PHOTONStream_PHOTONFilter" )

#    topSequence += PrimaryDPDPrescaler( "DAOD_ZEEGAMMA_VertexFilter",
#                                        RequireAlgs = [ "DAOD_ZEEStream_ZeeFilter",
#                                                        "DAOD_PHOTONStream_PHOTONFilter" ] )
#                                        Prescale = primDiPhotonDPD.D2AODPhotonElectronFilter.Prescale )
#    diElectronPHOBookkeepFilters.append( "DAOD_ZEEGAMMA_VertexFilter" )
#    diElectronPHOCMDString += "DAOD_ZEEGAMMA_VertexFilter"
#    pass    

#if diElectronPHOCMDString != "" and not diElectronPHOCMDString.endswith(" or "):
#    diElectronPHOCMDString += " or "   
#    pass

# Create the combined decission
topSequence += LogicalFilterCombiner( "StreamDAOD_DiElectronPHO_AcceptEvent",
                                      cmdstring = "DAOD_ZEEStream_ZeeVertexFilter and DAOD_PHOTONStream_PHOTONFilter" )
diElectronPHOBookkeepFilters.append( "StreamDAOD_DiElectronPHO_AcceptEvent" )


##====================================================================
## Define the DAOD Zee output stream
##====================================================================
streamName =primDPD.WriteDAOD_ZEEGAMMAStream.StreamName 
fileName   = buildFileName( primDPD.WriteDAOD_ZEEGAMMAStream )
if primDPD.isVirtual() == False:
    DAOD_EGammaStream=MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DAOD_EGammaStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
DAOD_EGammaStream.AddOtherAlgsToBookkeep( diElectronPHOBookkeepFilters )

DAOD_EGammaStream.AcceptAlgs( [ "StreamDAOD_DiElectronPHO_AcceptEvent" ] )
#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
