##-----------------------------------------------------------------------------
## Name: D2ESD_DiPhoton.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the Di Photon DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/D2ESD_DiPhoton.py")


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
if primDiPhotonDPD.DiPhotonFilter():
    topSequence += D2PDPhotonSelector( "DiPhotonStream_DiPhotonClusterFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'DiPhotonStream_DiPhotonClusterLinkCollection',
                                       minNumberPassed      = 2,
                                       clusterEtMin         = primDiPhotonDPD.DiPhotonFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.DiPhotonFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.DiPhotonFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.DiPhotonFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiPhotonClusterFilter" )
    
    topSequence += PrimaryDPDPrescaler( "DiPhotonStream_DiPhotonClusterPrescaleFilter",
                                        AcceptAlgs = ["DiPhotonStream_DiPhotonClusterFilter"],
                                        Prescale = primDiPhotonDPD.DiPhotonFilter.Prescale )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiPhotonClusterPrescaleFilter" )
    diPhCMDString += "DiPhotonStream_DiPhotonClusterPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define the di-photon filter (use photon Et)
if primDiPhotonDPD.DiPhotonFilter():
    topSequence += D2PDPhotonSelector( "DiPhotonStream_DiPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'DiPhotonStream_DiPhotonLinkCollection',
                                       minNumberPassed      = 2,
                                       etMin                = primDiPhotonDPD.DiPhotonFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.DiPhotonFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.DiPhotonFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.DiPhotonFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiPhotonFilter" )
    
    topSequence += PrimaryDPDPrescaler( "DiPhotonStream_DiPhotonPrescaleFilter",
                                        AcceptAlgs = ["DiPhotonStream_DiPhotonFilter"],
                                        Prescale = primDiPhotonDPD.DiPhotonFilter.Prescale )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiPhotonPrescaleFilter" )
    diPhCMDString += "DiPhotonStream_DiPhotonPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define the electron + photon filter (use cluter Et)
if primDiPhotonDPD.PhotonElectronFilter():
    topSequence += D2PDPhotonSelector( "DiPhotonStream_ElectronPhotonClusterPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'DiPhotonStream_ElectronPhotonClusterPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primDiPhotonDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.PhotonElectronFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.PhotonElectronFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "DiPhotonStream_ElectronPhotonClusterPhotonFilter" )

    topSequence += D2PDPhotonSelector( "DiPhotonStream_PhotonAuthorRConvClusterFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'DiPhotonStream_PhotonAuthorRConvClusterLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primDiPhotonDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.PhotonElectronFilter.PhotonID,
                                       photonAuthor         = egammaParameters.AuthorRConv
                                       )
    diPhotonBookkeepFilters.append( "DiPhotonStream_PhotonAuthorRConvClusterFilter" )

#    # Set up a DeltaR association/matching tool
#    ToolSvc += DeltaRAssociationTool( "DiPhotonStream_ElectronPhotonDeltaRAssociationTool",
#                                      OutputLevel                = INFO,
#                                      inputAssociateToCollection = "DiPhotonStream_PhotonAuthorRConvLinkCollection",
#                                      useClusterPosition         = True,
#                                      deltaRMax                  = 0.01,
#                                      writeUserData              = False
#                                      )

    # Set up a DeltaR association/matching tool
    ToolSvc += AthenaBarCodeAssociationTool( "DiPhotonStream_ElectronPhotonClusterAthenaBarCodeAssociationTool",
                                             OutputLevel                = INFO,
                                             inputAssociateToCollection = "DiPhotonStream_PhotonAuthorRConvClusterLinkCollection",
                                             writeUserData              = False
                                             )


    topSequence += D2PDElectronSelector( "DiPhotonStream_ElectronPhotonClusterElectronFilter",
                                         OutputLevel                    = INFO,
                                         inputCollection                = 'ElectronAODCollection',
                                         outputLinkCollection           = 'DiPhotonStream_ElectronPhotonClusterElectronLinkCollection',
                                         minNumberPassed                = 1,
                                         associationToolList            = [ ToolSvc.DiPhotonStream_ElectronPhotonClusterAthenaBarCodeAssociationTool ],
                                         outputAssociationContainerList = [ "DiPhotonStream_ElectronPhotonClusterAssociations" ],
                                         numberOfAssociationsMaxCutList = [ 0 ],
                                         #electronVetoAuthorList         = primDiPhotonDPD.PhotonElectronFilter.VetoElectronAuthor,
                                         electronID                     = primDiPhotonDPD.PhotonElectronFilter.ElectronID,
                                         clusterEtMin                   = primDiPhotonDPD.PhotonElectronFilter.ElectronEt,
                                         clusterEtaMax                  = primDiPhotonDPD.PhotonElectronFilter.ElectronEta
                                         )
    diPhotonBookkeepFilters.append( "DiPhotonStream_ElectronPhotonClusterElectronFilter" )
    
    
    topSequence += PrimaryDPDPrescaler( "DiPhotonStream_ElectronPhotonClusterPrescaleFilter",
                                        RequireAlgs = [ "DiPhotonStream_ElectronPhotonClusterElectronFilter",
                                                        "DiPhotonStream_ElectronPhotonClusterPhotonFilter" ],
                                        Prescale = primDiPhotonDPD.PhotonElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "DiPhotonStream_ElectronPhotonClusterPrescaleFilter" )
    diPhCMDString += "DiPhotonStream_ElectronPhotonClusterPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define the electron + photon filter (use object Et)
if primDiPhotonDPD.PhotonElectronFilter():
    topSequence += D2PDPhotonSelector( "DiPhotonStream_ElectronPhotonPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'DiPhotonStream_ElectronPhotonPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       etMin                = primDiPhotonDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.PhotonElectronFilter.PhotonID,
                                       photonAuthor         = primDiPhotonDPD.PhotonElectronFilter.PhotonAuthor
                                       )
    diPhotonBookkeepFilters.append( "DiPhotonStream_ElectronPhotonPhotonFilter" )

    topSequence += D2PDPhotonSelector( "DiPhotonStream_PhotonAuthorRConvFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'DiPhotonStream_PhotonAuthorRConvLinkCollection',
                                       minNumberPassed      = 1,
                                       etMin                = primDiPhotonDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primDiPhotonDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primDiPhotonDPD.PhotonElectronFilter.PhotonID,
                                       photonAuthor         = egammaParameters.AuthorRConv
                                       )
    diPhotonBookkeepFilters.append( "DiPhotonStream_PhotonAuthorRConvFilter" )


    # Set up a DeltaR association/matching tool
    ToolSvc += AthenaBarCodeAssociationTool( "DiPhotonStream_ElectronPhotonAthenaBarCodeAssociationTool",
                                             OutputLevel                = INFO,
                                             inputAssociateToCollection = "DiPhotonStream_PhotonAuthorRConvLinkCollection",
                                             writeUserData              = False
                                      )


    topSequence += D2PDElectronSelector( "DiPhotonStream_ElectronPhotonElectronFilter",
                                         OutputLevel                    = INFO,
                                         inputCollection                = 'ElectronAODCollection',
                                         outputLinkCollection           = 'DiPhotonStream_ElectronPhotonElectronLinkCollection',
                                         minNumberPassed                = 1,
                                         associationToolList            = [ ToolSvc.DiPhotonStream_ElectronPhotonAthenaBarCodeAssociationTool ],
                                         outputAssociationContainerList = [ "DiPhotonStream_ElectronPhotonAssociations" ],
                                         numberOfAssociationsMaxCutList = [ 0 ],
                                         #electronVetoAuthorList         = primDiPhotonDPD.PhotonElectronFilter.VetoElectronAuthor,
                                         electronID                     = primDiPhotonDPD.PhotonElectronFilter.ElectronID,
                                         etMin                          = primDiPhotonDPD.PhotonElectronFilter.ElectronEt,
                                         clusterEtaMax                  = primDiPhotonDPD.PhotonElectronFilter.ElectronEta
                                         )
    diPhotonBookkeepFilters.append( "DiPhotonStream_ElectronPhotonElectronFilter" )
    
    
    topSequence += PrimaryDPDPrescaler( "DiPhotonStream_ElectronPhotonPrescaleFilter",
                                        RequireAlgs = [ "DiPhotonStream_ElectronPhotonElectronFilter",
                                                        "DiPhotonStream_ElectronPhotonPhotonFilter" ],
                                        Prescale = primDiPhotonDPD.PhotonElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "DiPhotonStream_ElectronPhotonPrescaleFilter" )
    diPhCMDString += "DiPhotonStream_ElectronPhotonPrescaleFilter"
    pass



if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define a di-electron filter (use cluter Et)
if primDiPhotonDPD.DiElectronFilter():
    topSequence += D2PDElectronSelector( "DiPhotonStream_DiElectronClusterFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DiPhotonStream_DiElectronClusterLinkCollection',
                                         minNumberPassed        = 2,
                                         #electronVetoAuthorList = primDiPhotonDPD.DiElectronFilter.VetoElectronAuthor,
                                         electronID             = primDiPhotonDPD.DiElectronFilter.ElectronID,
                                         clusterEtMin           = primDiPhotonDPD.DiElectronFilter.ElectronEt
                                         )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiElectronClusterFilter" )
    
    # Define the prescaled filters; configurable with command line argument
    topSequence += PrimaryDPDPrescaler( "DiPhotonStream_DiElectronClusterPrescaleFilter",
                                        AcceptAlgs = ["DiPhotonStream_DiElectronClusterFilter"],
                                        Prescale = primDiPhotonDPD.DiElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiElectronClusterPrescaleFilter" )
    diPhCMDString += "DiPhotonStream_DiElectronClusterPrescaleFilter"
    pass


if diPhCMDString != "" and not diPhCMDString.endswith(" or "):
    diPhCMDString += " or "
    pass



# Define a di-electron filter (use electron Et)
if primDiPhotonDPD.DiElectronFilter():
    topSequence += D2PDElectronSelector( "DiPhotonStream_DiElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DiPhotonStream_DiElectronLinkCollection',
                                         minNumberPassed        = 2,
                                         #electronVetoAuthorList = primDiPhotonDPD.DiElectronFilter.VetoElectronAuthor,
                                         electronID             = primDiPhotonDPD.DiElectronFilter.ElectronID,
                                         etMin                  = primDiPhotonDPD.DiElectronFilter.ElectronEt
                                         )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiElectronFilter" )
    
    # Define the prescaled filters; configurable with command line argument
    topSequence += PrimaryDPDPrescaler( "DiPhotonStream_DiElectronPrescaleFilter",
                                        AcceptAlgs = ["DiPhotonStream_DiElectronFilter"],
                                        Prescale = primDiPhotonDPD.DiElectronFilter.Prescale )
    diPhotonBookkeepFilters.append( "DiPhotonStream_DiElectronPrescaleFilter" )
    diPhCMDString += "DiPhotonStream_DiElectronPrescaleFilter"
    pass



if diPhCMDString.endswith(" or "):
    diPhCMDString = diPhCMDString[:-len(" or ")]
    pass



# Create the combined decission
topSequence += LogicalFilterCombiner( "StreamD2ESD_DIPHO_AcceptEvent",
                                      cmdstring = diPhCMDString )
diPhotonBookkeepFilters.append( "StreamD2ESD_DIPHO_AcceptEvent" )




##====================================================================
## Define the Single Electron DPD output stream
##====================================================================
streamName = primDPD.WriteDiPhotonStream.StreamName
fileName   = buildFileName( primDPD.WriteDiPhotonStream )
if primDPD.isVirtual() == False:
    DiPhotonStream=MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DiPhotonStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
DiPhotonStream.AddOtherAlgsToBookkeep( diPhotonBookkeepFilters )

if primDPD.ApplySkimming() and primDiPhotonDPD.ApplySkimming() :
    # Schedule these prescaled algorithms as filters in OR
    DiPhotonStream.AcceptAlgs( [ "StreamD2ESD_DIPHO_AcceptEvent" ] )
    pass




#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
#DiPhotonStream.AddItem( ["egammaContainer#ElectronAODCollection"] )

