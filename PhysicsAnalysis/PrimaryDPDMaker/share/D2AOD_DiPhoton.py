##-----------------------------------------------------------------------------
## Name: D2AOD_DiPhoton.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the Di Photon DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/D2AOD_DiPhoton.py")


## for messaging
from AthenaCommon.Logging import logging
diphotonDPDStream_msg = logging.getLogger( 'PrimaryDPD_PerfDiPhotonStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_DiPhotonStream import primDiPhotonDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
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



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    diphotonDPDStream_msg.info('Values of all PrimaryDPDFlags_DiPhotonStream flags:')    
    print primDiPhotonDPD
    pass





##============================================================================
## Define the skimming (event selection) for the DESD_DIPHO output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
from AssociationComps.AssociationCompsConf import AthenaBarCodeAssociationTool
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector

# Create the command string for the final decision
diPhCMDString = ""

# Create the list of filter algorithms to bookkeep
diPhotonBookkeepFilters = []

# Define the di-photon filter (use cluter Et)
if primDiPhotonDPD.D2AODDiPhotonFilter():
    topSequence += D2PDPhotonSelector( "D2AOD_DIPHOStream_DiPhotonClusterFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'D2AOD_DIPHOStream_DiPhotonClusterLinkCollection',
                                       minNumberPassed      = 2,
                                       clusterEtMin         = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiPhotonClusterFilter" )
    
    topSequence += PrimaryDPDPrescaler( "D2AOD_DIPHOStream_DiPhotonClusterPrescaleFilter",
                                        AcceptAlgs = ["D2AOD_DIPHOStream_DiPhotonClusterFilter"],
                                        Prescale = primDiPhotonDPD.D2AODDiPhotonFilter.Prescale )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiPhotonClusterPrescaleFilter" )
    diPhCMDString += "D2AOD_DIPHOStream_DiPhotonClusterPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define the di-photon filter (use photon Et)
if primDiPhotonDPD.D2AODDiPhotonFilter():
    topSequence += D2PDPhotonSelector( "D2AOD_DIPHOStream_DiPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'D2AOD_DIPHOStream_DiPhotonLinkCollection',
                                       minNumberPassed      = 2,
                                       etMin                = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.D2AODDiPhotonFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiPhotonFilter" )
    
    topSequence += PrimaryDPDPrescaler( "D2AOD_DIPHOStream_DiPhotonPrescaleFilter",
                                        AcceptAlgs = ["D2AOD_DIPHOStream_DiPhotonFilter"],
                                        Prescale = primDiPhotonDPD.D2AODDiPhotonFilter.Prescale )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiPhotonPrescaleFilter" )
    diPhCMDString += "D2AOD_DIPHOStream_DiPhotonPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define the electron + photon filter (use cluter Et)
if primDiPhotonDPD.D2AODPhotonElectronFilter():
    topSequence += D2PDPhotonSelector( "D2AOD_DIPHOStream_ElectronPhotonClusterPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'D2AOD_DIPHOStream_ElectronPhotonClusterPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_ElectronPhotonClusterPhotonFilter" )

    topSequence += D2PDPhotonSelector( "D2AOD_DIPHOStream_PhotonAuthorRConvClusterFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'D2AOD_DIPHOStream_PhotonAuthorRConvClusterLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonID,
                                       photonAuthor         = egammaParameters.AuthorRConv
                                       )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_PhotonAuthorRConvClusterFilter" )

#    # Set up a DeltaR association/matching tool
#    ToolSvc += DeltaRAssociationTool( "D2AOD_DIPHOStream_ElectronPhotonDeltaRAssociationTool",
#                                      OutputLevel                = INFO,
#                                      inputAssociateToCollection = "D2AOD_DIPHOStream_PhotonAuthorRConvLinkCollection",
#                                      useClusterPosition         = True,
#                                      deltaRMax                  = 0.01,
#                                      writeUserData              = False
#                                      )

    # Set up a DeltaR association/matching tool
    ToolSvc += AthenaBarCodeAssociationTool( "D2AOD_DIPHOStream_ElectronPhotonClusterAthenaBarCodeAssociationTool",
                                             OutputLevel                = INFO,
                                             inputAssociateToCollection = "D2AOD_DIPHOStream_PhotonAuthorRConvClusterLinkCollection",
                                             writeUserData              = False
                                             )


    topSequence += D2PDElectronSelector( "D2AOD_DIPHOStream_ElectronPhotonClusterElectronFilter",
                                         OutputLevel                    = INFO,
                                         inputCollection                = 'ElectronAODCollection',
                                         outputLinkCollection           = 'D2AOD_DIPHOStream_ElectronPhotonClusterElectronLinkCollection',
                                         minNumberPassed                = 1,
                                         associationToolList            = [ ToolSvc.D2AOD_DIPHOStream_ElectronPhotonClusterAthenaBarCodeAssociationTool ],
                                         outputAssociationContainerList = [ "D2AOD_DIPHOStream_ElectronPhotonClusterAssociations" ],
                                         numberOfAssociationsMaxCutList = [ 0 ],
                                         #electronVetoAuthorList         = primDiPhotonDPD.D2AODPhotonElectronFilter.VetoElectronAuthor,
                                         electronID                     = primDiPhotonDPD.D2AODPhotonElectronFilter.ElectronID,
                                         clusterEtMin                   = primDiPhotonDPD.D2AODPhotonElectronFilter.ElectronEt,
                                         clusterEtaMax                  = primDiPhotonDPD.D2AODPhotonElectronFilter.ElectronEta
                                         )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_ElectronPhotonClusterElectronFilter" )
    
    
    topSequence += PrimaryDPDPrescaler( "D2AOD_DIPHOStream_ElectronPhotonClusterPrescaleFilter",
                                        RequireAlgs = [ "D2AOD_DIPHOStream_ElectronPhotonClusterElectronFilter",
                                                        "D2AOD_DIPHOStream_ElectronPhotonClusterPhotonFilter" ],
                                        Prescale = primDiPhotonDPD.D2AODPhotonElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_ElectronPhotonClusterPrescaleFilter" )
    diPhCMDString += "D2AOD_DIPHOStream_ElectronPhotonClusterPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define the electron + photon filter (use object Et)
if primDiPhotonDPD.D2AODPhotonElectronFilter():
    topSequence += D2PDPhotonSelector( "D2AOD_DIPHOStream_ElectronPhotonPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'D2AOD_DIPHOStream_ElectronPhotonPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       etMin                = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_ElectronPhotonPhotonFilter" )

    topSequence += D2PDPhotonSelector( "D2AOD_DIPHOStream_PhotonAuthorRConvFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'D2AOD_DIPHOStream_PhotonAuthorRConvLinkCollection',
                                       minNumberPassed      = 1,
                                       etMin                = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.D2AODPhotonElectronFilter.PhotonID,
                                       photonAuthor         = egammaParameters.AuthorRConv
                                       )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_PhotonAuthorRConvFilter" )


    # Set up a DeltaR association/matching tool
    ToolSvc += AthenaBarCodeAssociationTool( "D2AOD_DIPHOStream_ElectronPhotonAthenaBarCodeAssociationTool",
                                             OutputLevel                = INFO,
                                             inputAssociateToCollection = "D2AOD_DIPHOStream_PhotonAuthorRConvLinkCollection",
                                             writeUserData              = False
                                      )


    topSequence += D2PDElectronSelector( "D2AOD_DIPHOStream_ElectronPhotonElectronFilter",
                                         OutputLevel                    = INFO,
                                         inputCollection                = 'ElectronAODCollection',
                                         outputLinkCollection           = 'D2AOD_DIPHOStream_ElectronPhotonElectronLinkCollection',
                                         minNumberPassed                = 1,
                                         associationToolList            = [ ToolSvc.D2AOD_DIPHOStream_ElectronPhotonAthenaBarCodeAssociationTool ],
                                         outputAssociationContainerList = [ "D2AOD_DIPHOStream_ElectronPhotonAssociations" ],
                                         numberOfAssociationsMaxCutList = [ 0 ],
                                         #electronVetoAuthorList         = primDiPhotonDPD.D2AODPhotonElectronFilter.VetoElectronAuthor,
                                         electronID                     = primDiPhotonDPD.D2AODPhotonElectronFilter.ElectronID,
                                         etMin                          = primDiPhotonDPD.D2AODPhotonElectronFilter.ElectronEt,
                                         clusterEtaMax                  = primDiPhotonDPD.D2AODPhotonElectronFilter.ElectronEta
                                         )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_ElectronPhotonElectronFilter" )
    
    
    topSequence += PrimaryDPDPrescaler( "D2AOD_DIPHOStream_ElectronPhotonPrescaleFilter",
                                        RequireAlgs = [ "D2AOD_DIPHOStream_ElectronPhotonElectronFilter",
                                                        "D2AOD_DIPHOStream_ElectronPhotonPhotonFilter" ],
                                        Prescale = primDiPhotonDPD.D2AODPhotonElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_ElectronPhotonPrescaleFilter" )
    diPhCMDString += "D2AOD_DIPHOStream_ElectronPhotonPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define a di-electron filter (use cluter Et)
if primDiPhotonDPD.D2AODDiElectronFilter():
    topSequence += D2PDElectronSelector( "D2AOD_DIPHOStream_DiElectronClusterFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'D2AOD_DIPHOStream_DiElectronClusterLinkCollection',
                                         minNumberPassed        = 2,
                                         #electronVetoAuthorList = primDiPhotonDPD.D2AODDiElectronFilter.VetoElectronAuthor,
                                         electronID             = primDiPhotonDPD.D2AODDiElectronFilter.ElectronID,
                                         clusterEtMin           = primDiPhotonDPD.D2AODDiElectronFilter.ElectronEt
                                         )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiElectronClusterFilter" )
    
    # Define the prescaled filters; configurable with command line argument
    topSequence += PrimaryDPDPrescaler( "D2AOD_DIPHOStream_DiElectronClusterPrescaleFilter",
                                        AcceptAlgs = ["D2AOD_DIPHOStream_DiElectronClusterFilter"],
                                        Prescale = primDiPhotonDPD.D2AODDiElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiElectronClusterPrescaleFilter" )
    diPhCMDString += "D2AOD_DIPHOStream_DiElectronClusterPrescaleFilter"
    pass


if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define a di-electron filter (use electron Et)
if primDiPhotonDPD.D2AODDiElectronFilter():
    topSequence += D2PDElectronSelector( "D2AOD_DIPHOStream_DiElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'D2AOD_DIPHOStream_DiElectronLinkCollection',
                                         minNumberPassed        = 2,
                                         #electronVetoAuthorList = primDiPhotonDPD.D2AODDiElectronFilter.VetoElectronAuthor,
                                         electronID             = primDiPhotonDPD.D2AODDiElectronFilter.ElectronID,
                                         etMin                  = primDiPhotonDPD.D2AODDiElectronFilter.ElectronEt
                                         )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiElectronFilter" )
    
    # Define the prescaled filters; configurable with command line argument
    topSequence += PrimaryDPDPrescaler( "D2AOD_DIPHOStream_DiElectronPrescaleFilter",
                                        AcceptAlgs = ["D2AOD_DIPHOStream_DiElectronFilter"],
                                        Prescale = primDiPhotonDPD.D2AODDiElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "D2AOD_DIPHOStream_DiElectronPrescaleFilter" )
    diPhCMDString += "D2AOD_DIPHOStream_DiElectronPrescaleFilter"
    pass



if diPhCMDString.endswith(" or "):
    diPhCMDString = diPhCMDString[:-len(" or ")]
    pass



# Create the combined decission
topSequence += LogicalFilterCombiner( "StreamD2AOD_DIPHO_AcceptEvent",
                                      cmdstring = diPhCMDString )
diPhotonBookkeepFilters.append( "StreamD2AOD_DIPHO_AcceptEvent" )




##====================================================================
## Define the Single Electron DPD output stream
##====================================================================
streamName = primDPD.WriteD2AOD_DIPHOStream.StreamName
fileName   = buildFileName( primDPD.WriteD2AOD_DIPHOStream )
if primDPD.isVirtual() == False:
    D2AOD_DIPHOStream=MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    D2AOD_DIPHOStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
D2AOD_DIPHOStream.AddOtherAlgsToBookkeep( diPhotonBookkeepFilters )

if primDPD.ApplySkimming() and primDiPhotonDPD.ApplySkimming() :
    # Schedule these prescaled algorithms as filters in OR
    D2AOD_DIPHOStream.AcceptAlgs( [ "StreamD2AOD_DIPHO_AcceptEvent" ] )
    pass




#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
#D2AOD_DIPHOStream.AddItem( ["egammaContainer#ElectronAODCollection"] )

