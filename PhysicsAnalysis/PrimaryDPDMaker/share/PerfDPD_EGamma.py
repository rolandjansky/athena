##-----------------------------------------------------------------------------
## Name: PrimaryDPD_EGammaStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the EGamma DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_EGamma.py")

## for messaging
from AthenaCommon.Logging import logging
egammaDPDStream_msg = logging.getLogger( 'PrimaryDPD_EGammaStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_EGammaStream import primEGammaDPD
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
    egammaDPDStream_msg.info('Values of all PrimaryDPDFlags_EGammaStream flags:')    
    print primEGammaDPD
    pass




# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdEGammaSequence = CfgMgr.AthSequencer( 'DESDM_EGAMMA_Sequence',
                                          StopOverride = True )
topSequence += desdEGammaSequence



##============================================================================
## Define the skimming (event selection) for the DESDM_EGAMMA output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
from D2PDMaker.D2PDMakerConf import D2PDINavigable4MomentumSelector
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner

# Set up the command string that will contain the logical filter combination
egammaCMDString = ""
algsToBookkeep = []




# Do the trigger selection
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
if primEGammaDPD.JpsiTriggerFilter():
    for trigNamePrescale in primEGammaDPD.JpsiTriggerFilter.TriggerNames :
        TriggerFilterName         = "EGammaStream_JpsiTriggerFilter_"+trigNamePrescale[0]
        TriggerPrescaleFilterName = "EGammaStream_JpsiTriggerPrescaleFilter_"+trigNamePrescale[0]
        desdEGammaSequence += TriggerFilter( TriggerFilterName,
                                             trigger = trigNamePrescale[0]
                                             )
        desdEGammaSequence += PrimaryDPDPrescaler( TriggerPrescaleFilterName,
                                                   AcceptAlgs = [ TriggerFilterName ],
                                                   Prescale   = trigNamePrescale[1]
                                                   )
        # Add the per-event bookkeeping of this filer
        algsToBookkeep.append(TriggerPrescaleFilterName)

        egammaCMDString += TriggerPrescaleFilterName
        if egammaCMDString != "" and not egammaCMDString.endswith(" or "):
            egammaCMDString += " or "
            pass
        pass
    pass





# Define the Zee filter
if primEGammaDPD.ZeeFilter():
    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_ZeeElectron1Filter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DESD_EGammaStream_ZeeElectron1LinkCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = primEGammaDPD.ZeeFilter.VetoElectron1Author,
                                         electronID           = primEGammaDPD.ZeeFilter.Electron1IsEM,
                                         clusterEtMin           = primEGammaDPD.ZeeFilter.Electron1Et
                                         )
    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_ZeeElectron2Filter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DESD_EGammaStream_ZeeElectron2LinkCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = primEGammaDPD.ZeeFilter.VetoElectron2Author,
                                         electronID           = primEGammaDPD.ZeeFilter.Electron2IsEM,
                                         clusterEtMin           = primEGammaDPD.ZeeFilter.Electron2Et
                                         )
    
    # Define the Z -> ee filter
    desdEGammaSequence += D2PDParticleCombiner( "EGammaStream_ZeeFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'DESD_EGammaStream_ZeeElectron1LinkCollection',
                                                                     'DESD_EGammaStream_ZeeElectron2LinkCollection' ],
                                         outputCollection        = 'DESD_EGammaStream_ZeeBosonCollection',
                                         minNumberPassed         = 1
#                                         massMin                 = primEGammaDPD.ZeeFilter.MinMass
                                         )
    
    # Define the prescaled filters; configurable with command line argument
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_ZeePrescaleFilter",
                                        AcceptAlgs = ["EGammaStream_ZeeFilter"],
                                        Prescale = primEGammaDPD.ZeeFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("EGammaStream_ZeePrescaleFilter")
    egammaCMDString += "EGammaStream_ZeePrescaleFilter"    
    pass




if egammaCMDString != "" and not egammaCMDString.endswith(" or "):
    egammaCMDString += " or "
    pass





# Define a Tag and Probe electron filter
if primEGammaDPD.TagAndProbeElectronFilter():
    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_TagElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DESD_EGammaStream_TagElectronLinkCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = primEGammaDPD.TagAndProbeElectronFilter.TagVetoElectronAuthor,
                                         electronID           = primEGammaDPD.TagAndProbeElectronFilter.TagElectronIsEM,
                                         clusterEtMin           = primEGammaDPD.TagAndProbeElectronFilter.TagElectronEt,
                                         egammaTrackIsolationParameter = primEGammaDPD.TagAndProbeElectronFilter.TagElectronPtCone,
                                         relativeTrackPtIsolationMax   = primEGammaDPD.TagAndProbeElectronFilter.TagElectronPtConeMax
                                         )
    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_ProbeForwardElectronFilter",
                                         OutputLevel              = INFO,
                                         inputCollection          = 'ElectronAODCollection',
                                         outputLinkCollection     = 'DESD_EGammaStream_ProbeForwardElectronLinkCollection',
                                         minNumberPassed          = 1,
                                         electronAcceptAuthorList = primEGammaDPD.TagAndProbeElectronFilter.ProbeElectronAuthor,
                                         #electronID             = primEGammaDPD.TagAndProbeElectronFilter.ProbeElectronIsEM,
                                         clusterEtMin             = primEGammaDPD.TagAndProbeElectronFilter.ProbeElectronEt
                                         )
    desdEGammaSequence += D2PDINavigable4MomentumSelector( "EGammaStream_ProbeElectronFilter",
                                                    OutputLevel            = INFO,
                                                    inputCollection        = 'egClusterCollection',
                                                    outputLinkCollection   = 'DESD_EGammaStream_ProbeElectronLinkCollection',
                                                    minNumberPassed        = 1,
                                                    etMin                  = primEGammaDPD.TagAndProbeElectronFilter.ProbeElectronEt
                                                    )
    # Define the Tag-and-Probe combination
    desdEGammaSequence += D2PDParticleCombiner( "EGammaStream_TagAndProbeForwardElectronFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'DESD_EGammaStream_TagElectronLinkCollection',
                                                                     'DESD_EGammaStream_ProbeForwardElectronLinkCollection' ],
                                         outputCollection        = 'DESD_EGammaStream_TagAndProbeForwardZeeBosonCollection',
                                         massMin                 = primEGammaDPD.TagAndProbeElectronFilter.MinMass,
                                         minNumberPassed         = 1
                                         )
    desdEGammaSequence += D2PDParticleCombiner( "EGammaStream_TagAndProbeElectronFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'DESD_EGammaStream_TagElectronLinkCollection',
                                                                     'DESD_EGammaStream_ProbeElectronLinkCollection' ],
                                         outputCollection        = 'DESD_EGammaStream_TagAndProbeZeeBosonCollection',
                                         massMin                 = primEGammaDPD.TagAndProbeElectronFilter.MinMass,
                                         minNumberPassed         = 1
                                         )
    
    # Define the prescaled filters; configurable with command line argument
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_TagAndProbeElectronPrescaleFilter",
                                        AcceptAlgs = [ "EGammaStream_TagAndProbeElectronFilter",
                                                       "EGammaStream_TagAndProbeForwardElectronFilter" ],
                                        Prescale = primEGammaDPD.TagAndProbeElectronFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("EGammaStream_TagAndProbeElectronPrescaleFilter")
    egammaCMDString += "EGammaStream_TagAndProbeElectronPrescaleFilter"
    pass




if egammaCMDString != "" and not egammaCMDString.endswith(" or "):
    egammaCMDString += " or "
    pass






# Define the di-photon filter (use cluter Et)
if primEGammaDPD.DiPhotonFilter():
    desdEGammaSequence += D2PDPhotonSelector( "EGammaStream_DiPhotonClusterFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'EGammaStream_DiPhotonClusterLinkCollection',
                                       minNumberPassed      = 2,
                                       clusterEtMin         = primEGammaDPD.DiPhotonFilter.PhotonEt,
                                       clusterEtaMax        = primEGammaDPD.DiPhotonFilter.PhotonEta,
                                       photonID             = primEGammaDPD.DiPhotonFilter.PhotonIsEM,
                                       photonAuthor         = primEGammaDPD.DiPhotonFilter.PhotonAuthor
                                       )
    #diPhotonBookkeepFilters.append( "EGammaStream_DiPhotonClusterFilter" )
    
    desdEGammaSequence += D2PDPhotonSelector( "EGammaStream_DiPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'EGammaStream_DiPhotonLinkCollection',
                                       minNumberPassed      = 2,
                                       etMin                = primEGammaDPD.DiPhotonFilter.PhotonEt,
                                       clusterEtaMax        = primEGammaDPD.DiPhotonFilter.PhotonEta,
                                       photonID             = primEGammaDPD.DiPhotonFilter.PhotonIsEM,
                                       photonAuthor         = primEGammaDPD.DiPhotonFilter.PhotonAuthor
                                       )
    #diPhotonBookkeepFilters.append( "EGammaStream_DiPhotonFilter" )
    
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_DiPhotonPrescaleFilter",
                                        AcceptAlgs = [ "EGammaStream_DiPhotonClusterFilter",
                                                       "EGammaStream_DiPhotonFilter" ],
                                        Prescale = primEGammaDPD.DiPhotonFilter.Prescale )
    #diPhotonBookkeepFilters.append( "EGammaStream_DiPhotonPrescaleFilter" )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("EGammaStream_DiPhotonPrescaleFilter")
    egammaCMDString += "EGammaStream_DiPhotonPrescaleFilter"
    pass



if egammaCMDString != "" and not egammaCMDString.endswith(" or "):
    egammaCMDString += " or "
    pass



# Define the electron + photon filter (use cluter Et)
if primEGammaDPD.PhotonElectronFilter():
    desdEGammaSequence += D2PDPhotonSelector( "EGammaStream_ElectronPhotonClusterPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'EGammaStream_ElectronPhotonClusterPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primEGammaDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primEGammaDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primEGammaDPD.PhotonElectronFilter.PhotonIsEM,
                                       photonAuthor         = primEGammaDPD.PhotonElectronFilter.PhotonAuthor
                                       )
    #diPhotonBookkeepFilters.append( "EGammaStream_ElectronPhotonClusterPhotonFilter" )

    desdEGammaSequence += D2PDPhotonSelector( "EGammaStream_PhotonAuthorRConvClusterFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'EGammaStream_PhotonAuthorRConvClusterLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primEGammaDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primEGammaDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primEGammaDPD.PhotonElectronFilter.PhotonIsEM,
                                       photonAuthor         = egammaParameters.AuthorRConv
                                       )
    #diPhotonBookkeepFilters.append( "EGammaStream_PhotonAuthorRConvClusterFilter" )


    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import AthenaBarCodeAssociationTool
    ToolSvc += AthenaBarCodeAssociationTool( "EGammaStream_ElectronPhotonClusterAthenaBarCodeAssociationTool",
                                             OutputLevel                = INFO,
                                             inputAssociateToCollection = "EGammaStream_PhotonAuthorRConvClusterLinkCollection",
                                             writeUserData              = False
                                             )


    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_ElectronPhotonClusterElectronFilter",
                                         OutputLevel                    = INFO,
                                         inputCollection                = 'ElectronAODCollection',
                                         outputLinkCollection           = 'EGammaStream_ElectronPhotonClusterElectronLinkCollection',
                                         minNumberPassed                = 1,
                                         associationToolList            = [ ToolSvc.EGammaStream_ElectronPhotonClusterAthenaBarCodeAssociationTool ],
                                         outputAssociationContainerList = [ "EGammaStream_ElectronPhotonClusterAssociations" ],
                                         numberOfAssociationsMaxCutList = [ 0 ],
                                         #electronVetoAuthorList         = primEGammaDPD.PhotonElectronFilter.VetoElectronAuthor,
                                         electronID                     = primEGammaDPD.PhotonElectronFilter.ElectronIsEM,
                                         clusterEtMin                   = primEGammaDPD.PhotonElectronFilter.ElectronEt,
                                         clusterEtaMax                  = primEGammaDPD.PhotonElectronFilter.ElectronEta
                                         )
    #diPhotonBookkeepFilters.append( "EGammaStream_ElectronPhotonClusterElectronFilter" )
    
    
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_ElectronPhotonClusterFilter",
                                        RequireAlgs = [ "EGammaStream_ElectronPhotonClusterElectronFilter",
                                                        "EGammaStream_ElectronPhotonClusterPhotonFilter" ],
                                        Prescale = 1 )
    #diPhotonBookkeepFilters.append( "EGammaStream_ElectronPhotonClusterPrescaleFilter" )

    # Do the object selection based on the egamma object directly
    desdEGammaSequence += D2PDPhotonSelector( "EGammaStream_ElectronPhotonPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'EGammaStream_ElectronPhotonPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       etMin                = primEGammaDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primEGammaDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primEGammaDPD.PhotonElectronFilter.PhotonIsEM,
                                       photonAuthor         = primEGammaDPD.PhotonElectronFilter.PhotonAuthor
                                       )
    #diPhotonBookkeepFilters.append( "EGammaStream_ElectronPhotonPhotonFilter" )

    desdEGammaSequence += D2PDPhotonSelector( "EGammaStream_PhotonAuthorRConvFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'EGammaStream_PhotonAuthorRConvLinkCollection',
                                       minNumberPassed      = 1,
                                       etMin                = primEGammaDPD.PhotonElectronFilter.PhotonEt,
                                       clusterEtaMax        = primEGammaDPD.PhotonElectronFilter.PhotonEta,
                                       photonID             = primEGammaDPD.PhotonElectronFilter.PhotonIsEM,
                                       photonAuthor         = egammaParameters.AuthorRConv
                                       )
    #diPhotonBookkeepFilters.append( "EGammaStream_PhotonAuthorRConvFilter" )


    # Set up a DeltaR association/matching tool
    ToolSvc += AthenaBarCodeAssociationTool( "EGammaStream_ElectronPhotonAthenaBarCodeAssociationTool",
                                             OutputLevel                = INFO,
                                             inputAssociateToCollection = "EGammaStream_PhotonAuthorRConvLinkCollection",
                                             writeUserData              = False
                                      )


    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_ElectronPhotonElectronFilter",
                                         OutputLevel                    = INFO,
                                         inputCollection                = 'ElectronAODCollection',
                                         outputLinkCollection           = 'EGammaStream_ElectronPhotonElectronLinkCollection',
                                         minNumberPassed                = 1,
                                         associationToolList            = [ ToolSvc.EGammaStream_ElectronPhotonAthenaBarCodeAssociationTool ],
                                         outputAssociationContainerList = [ "EGammaStream_ElectronPhotonAssociations" ],
                                         numberOfAssociationsMaxCutList = [ 0 ],
                                         #electronVetoAuthorList         = primEGammaDPD.PhotonElectronFilter.VetoElectronAuthor,
                                         electronID                   = primEGammaDPD.PhotonElectronFilter.ElectronIsEM,
                                         etMin                          = primEGammaDPD.PhotonElectronFilter.ElectronEt,
                                         clusterEtaMax                  = primEGammaDPD.PhotonElectronFilter.ElectronEta
                                         )
    #diPhotonBookkeepFilters.append( "EGammaStream_ElectronPhotonElectronFilter" )
        
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_ElectronPhotonEGammaFilter",
                                        RequireAlgs = [ "EGammaStream_ElectronPhotonElectronFilter",
                                                        "EGammaStream_ElectronPhotonPhotonFilter" ],
                                        Prescale = 1 )

    # Create the combination
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_ElectronPhotonPrescaleFilter",
                                        AcceptAlgs = [ "EGammaStream_ElectronPhotonClusterFilter",
                                                       "EGammaStream_ElectronPhotonEGammaFilter" ],
                                        Prescale = primEGammaDPD.PhotonElectronFilter.Prescale )
    #diPhotonBookkeepFilters.append( "EGammaStream_ElectronPhotonPrescaleFilter" )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("EGammaStream_ElectronPhotonPrescaleFilter")
    egammaCMDString += "EGammaStream_ElectronPhotonPrescaleFilter"
    pass



if egammaCMDString != "" and not egammaCMDString.endswith(" or "):
    egammaCMDString += " or "
    pass



# Define a di-electron filter (use cluter Et)
if primEGammaDPD.DiElectronFilter():
    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_DiElectronClusterFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'EGammaStream_DiElectronClusterLinkCollection',
                                         minNumberPassed        = 2,
                                         #electronVetoAuthorList = primEGammaDPD.DiElectronFilter.VetoElectronAuthor,
                                         electronID           = primEGammaDPD.DiElectronFilter.ElectronIsEM,
                                         clusterEtMin           = primEGammaDPD.DiElectronFilter.ElectronEt
                                         )
    #diPhotonBookkeepFilters.append( "EGammaStream_DiElectronClusterFilter" )
    
    desdEGammaSequence += D2PDElectronSelector( "EGammaStream_DiElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'EGammaStream_DiElectronLinkCollection',
                                         minNumberPassed        = 2,
                                         #electronVetoAuthorList = primEGammaDPD.DiElectronFilter.VetoElectronAuthor,
                                         electronID           = primEGammaDPD.DiElectronFilter.ElectronIsEM,
                                         etMin                  = primEGammaDPD.DiElectronFilter.ElectronEt
                                         )
    #diPhotonBookkeepFilters.append( "EGammaStream_DiElectronFilter" )
    
    # Define the prescaled filters; configurable with command line argument
    desdEGammaSequence += PrimaryDPDPrescaler( "EGammaStream_DiElectronPrescaleFilter",
                                        AcceptAlgs = [ "EGammaStream_DiElectronClusterFilter",
                                                       "EGammaStream_DiElectronFilter" ],
                                        Prescale = primEGammaDPD.DiElectronFilter.Prescale )
    #diPhotonBookkeepFilters.append( "EGammaStream_DiElectronPrescaleFilter" )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("EGammaStream_DiElectronPrescaleFilter")
    egammaCMDString += "EGammaStream_DiElectronPrescaleFilter"
    pass




if egammaCMDString.endswith(" or "):
    egammaCMDString = egammaCMDString[:-len(" or ")]
    pass


# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
thinSeqDESDM_EGAMMA = AthSequencer( "DESDM_EGAMMA_ThinningSequence" )
desdEGammaSequence += thinSeqDESDM_EGAMMA

# Create the combined decission
thinSeqDESDM_EGAMMA += LogicalFilterCombiner( "StreamDESDM_EGAMMA_AcceptEvent",
                                              OutputLevel = INFO,
                                              cmdstring = egammaCMDString )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESDM_EGAMMA_AcceptEvent" )


##====================================================================
## Define the thinning for the egamma DPD output stream
##====================================================================
if primDPD.ApplyThinning() and primEGammaDPD.ApplyThinning() and not primDPD.WriteEGammaStream.isVirtual and not primDPD.isVirtual() :
    #---------------------------------------------------
    # EGamma Track thinning using the egammaTrackSlimmer (only available on ESD)
    #---------------------------------------------------
    if rec.readESD() and  primEGammaDPD.ApplyEGammaTrackThinning() :
        from egammaRec.egammaRecConf import egammaTrackSlimmer
        thinSeqDESDM_EGAMMA += egammaTrackSlimmer( "ThinEGammaTracksInEGammaStream",
                                                   OutputLevel = INFO,
                                                   thinSvc                    = 'ThinningSvc/EGammaStreamThinning',
                                                   InputElectronContainerName = 'ElectronAODCollection',
                                                   InputPhotonContainerName   = 'PhotonAODCollection',
                                                   InDetSlimTrackContainer    = 'Tracks',
                                                   SaveConvThinTrk            = True )
        pass
    
    #---------------------------------------------------
    # Muon Track thinning using the MuonTrackSlimmer (only available on ESD)
    #---------------------------------------------------
    if rec.readESD() and  primEGammaDPD.ApplyMuonTrackThinning() :
        try:
            from MuonAlgs.MuonAlgsConf import Rec__MuonTrackSlimmer as DESDM_EGAMMA_MuonTrackSlimmer
            pass
        except ImportError:
            from MuonAlgs.MuonAlgsConf import MuonTrackSlimmer as DESDM_EGAMMA_MuonTrackSlimmer
            pass
        thinSeqDESDM_EGAMMA += DESDM_EGAMMA_MuonTrackSlimmer( "ThinMuonTracksInEGammaStream",
                                                              OutputLevel = INFO,
                                                              ThinService             = 'ThinningSvc/EGammaStreamThinning',
                                                              MuonTrackSlimmerTool    = 'Trk::TrackSlimmingTool/EGammaStreamMuonTrackSlimmerTool',
                                                              InDetSlimTrackContainer = 'Tracks',                                                            
                                                              doMuonTrackSlimming     = False )
        try:
            thinSeqDESDM_EGAMMA.ThinMuonTracksInEGammaStream.MuonContainerNames = [ "MuidMuonCollection",
                                                                                    "StacoMuonCollection",
                                                                                    "CaloMuonCollection",
                                                                                    "MuGirlLowBetaCollection" ]
            pass
        except AttributeError:
            egammaDPDStream_msg.info("Couldn't find property MuonContainerNames for ThinMuonTracksInEGammaStream. This is OK in releases < 17.0.0.")    
            pass
        pass
    

    #---------------------------------------------------
    # Tau Track thinning using the TauTrackSlimmer (only available on ESD)
    #---------------------------------------------------
    if rec.readESD() and  primEGammaDPD.ApplyTauTrackThinning() :
        from tauRec.tauRecConf import TauTrackSlimmer
        thinSeqDESDM_EGAMMA += TauTrackSlimmer( "ThinTauTracksInEGammaStream",
                                                OutputLevel = INFO,
                                                thinSvc      = 'ThinningSvc/EGammaStreamThinning',
                                                TauContainer = "TauRecContainer",
                                                FilterTaus   = True,
                                                maxNTrack    = 4,
                                                maxCharge    = 2,
                                                maxEmRadius  = 0.2,
                                                maxIsoFrac   = 0.5 )
        pass

    #---------------------------------------------------
    # L1Calo Trigger Tower Thinner (only available on ESD)
    #---------------------------------------------------
    if rec.readESD() and  primEGammaDPD.ApplyTriggerTowerThinning() :
        from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import TriggerTowerThinningAlg
        thinSeqDESDM_EGAMMA += TriggerTowerThinningAlg( "TriggerTowerThinnerInEGammaStream",
                                                        OutputLevel = INFO,
                                                        ThinService             = 'ThinningSvc/EGammaStreamThinning',
                                                        MinADC = 36,
                                                        MinCaloCellEnergy = 0.8,
                                                        UseRandom = True,
                                                        MinRandom = 0.01 )
        pass
    

    #---------------------------------------------------
    # CaloCell, PixelCluster, SCT_Cluster, and TRT_DriftCircle Thinning (only available on ESD)
    #---------------------------------------------------
    if primEGammaDPD.ApplyCaloCellThinning() \
       or primEGammaDPD.ApplyPixelClusterThinning() \
       or primEGammaDPD.ApplySCTClusterThinning() \
       or primEGammaDPD.ApplyTRTDriftCircleThinning() :
        # Example on how to use the C++ thinning algorithm. If you need to thin different containers
        #  with different selections, you need to create one instance per container to be thinned
        #  (with the SAME thinSvc name, but different instance name).
        from PrimaryDPDMaker.PrimaryDPDMakerConf import ThinContainers
        thinSeqDESDM_EGAMMA += ThinContainers( "ThinContainersInEGammaStream",
                                               OutputLevel = INFO,
                                               thinSvc     = 'ThinningSvc/EGammaStreamThinning' )
        
        if rec.readESD() and primEGammaDPD.ApplyCaloCellThinning() :
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.thinCaloCells               = True
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.caloCellCollectionName      = 'AllCalo'
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.keepCellsFromLinkContainers = True
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.cellLinkContainerNames      = [ "LArClusterEMSofte_Link", \
                                                                                            "egClusterCollection_Link", \
                                                                                            "LArClusterEM_Link" ]
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.keepForwardElectronCells    = True
            pass

        if rec.readESD() and primEGammaDPD.ApplyPixelClusterThinning() :
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.thinPixelClusters              = True
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.pixelClusterCollectionName     = 'PixelClusters'
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaEtaPixelElectronObject = 0.5
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaPhiPixelElectronObject = 0.5
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaZPixelElectronObject   = 100
            pass

        if rec.readESD() and primEGammaDPD.ApplySCTClusterThinning() :
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.thinSCTClusters              = True
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.sctClusterCollectionName     = 'SCT_Clusters'
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaEtaSCTElectronObject = 0.5
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaPhiSCTElectronObject = 0.5
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaZSCTElectronObject   = 100            
            pass

        if rec.readESD() and primEGammaDPD.ApplyTRTDriftCircleThinning() :
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.thinTRTDriftCircles           = True
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.trtDriftCirclesCollectionName = 'TRT_DriftCircles'
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaEtaTRTElectronObject  = 0.5
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaPhiTRTElectronObject  = 0.5
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaZTRTElectronObject    = 100            
            pass

        if rec.readESD() and ( primEGammaDPD.ApplyPixelClusterThinning() \
                               or primEGammaDPD.ApplySCTClusterThinning() \
                               or primEGammaDPD.ApplyTRTDriftCircleThinning() \
                               ) :
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.keepPRDFromTracks           = True
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.trackContainerNamesForPRD   = [ "Tracks" ]
            pass
        
        thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.keepObjectsNearElectrons    = True
        thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.minEtElectron               = 0.0*Units.GeV
        thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaRElectronObject     = 0.45
        
        thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.keepObjectsNearPhotons      = True
        thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.minEtPhoton                 = 0.0*Units.GeV
        thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.maxDeltaRPhotonObject       = 0.45
        pass

    pass  # End: if primDPD.ApplyThinning()





##====================================================================
## Define the egamma DPD output stream
##====================================================================
streamName = primDPD.WriteEGammaStream.StreamName
fileName   = buildFileName( primDPD.WriteEGammaStream )
if primDPD.WriteEGammaStream.isVirtual or primDPD.isVirtual() :
    EGammaStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    EGammaStream = MSMgr.NewPoolStream( streamName, fileName )
    # Add the per-event bookkeeping 
    EGammaStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass
    

if primDPD.ApplySkimming() and primEGammaDPD.ApplySkimming() :
    # Schedule the event selection
    EGammaStream.AcceptAlgs( [ "StreamDESDM_EGAMMA_AcceptEvent" ] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
excludeList = dpdOutput.excludeESDOnlyContainersList()
excludeList += [
    #'CaloClusterContainer#CaloCalTopoCluster',
    #'CaloClusterContainer#CaloTopoCluster',
    'CaloTopoTowerContainer#TopoTower',
    'CaloCellLinkContainer#CaloCalTopoCluster_Link',
    'CaloShowerContainer#CaloCalTopoCluster_Data',
    #'CaloShowerContainer#CaloTopoCluster_Data',
    #'CaloShowerContainer#EMTopoCluster430_Data',
    'InDet::Track',
    'Muon::MdtPrepDataContainer#MDT_DriftCircles',
    'Muon::RpcPrepDataContainer#RPC_Measurements',
    'Muon::TgcPrepDataContainer#TGC_MeasurementsNextBC',
    'Muon::TgcPrepDataContainer#TGC_MeasurementsPriorBC',
    'Muon::TgcPrepDataContainer#TGC_Measurements',
    'Muon::CscPrepDataContainer#CSC_Clusters',
    'Trk::SegmentCollection#ConvertedMBoySegments',
    'Trk::SegmentCollection#MooreSegments',
    'TrigMuonEFContainer#HLT_MuonEF' 
    ]
# Add more stuff to the exlude list while commissioning is still ongoing
excludeList += [
    'InDetRawDataContainer#TRT_RDOs',
    'TRT_RDO_Container#TRT_RDOs',
    'SCT_RawDataContainer#SCT_RDOs',
    'SCT_RDO_Container#SCT_RDOs',
    'InDetRawDataContainer#PixelRDOs',
    'PixelRDO_Container#PixelRDOs',
    'CTP_RDO#CTP_RDO',
    'MuCTPI_RDO#MUCTPI_RDO',
    'CscRawDataContainer#CSCRDO',
    'BCM_RDO_Container#BCM_RDOs',
    'TrackCollection#StandaloneTRTTracks',
    'TrackCollection#ResolvedSCTTracks',
    'TrackCollection#ResolvedPixelTracks',
    'MdtCsmContainer#MDTCSM',
    'RpcPadContainer#RPCPAD',
    'TrackCollection#MuTagIMOTracks',
    #'CaloCellLinkContainer#EMTopoCluster430_Link',
    'DataVector<LVL1::TriggerTower>#TriggerTowersMuon',
    'TileL2Container#TileL2Cnt',
    'LArDigitContainer#LArDigitContainer_EMClust',
    'TileDigitsContainer#TileDigitsFlt',
    'TauDetailsContainer#TauRecExtraDetailsContainer',
    'ZdcDigitsCollection#ZdcDigitsCollection',
    #'CaloCellLinkContainer#CaloTopoCluster_Link',
    'DataVector<LVL1::RODHeader>#RODHeaders',
    'ZdcRawChannelCollection#ZdcRawChannelCollection',
    'LArFebErrorSummary#LArFebErrorSummary',
    'Muon::ChamberT0s#MboyMuonChamberT0s',
    'Muon::ChamberT0s#MooreMuonChamberT0s',
    'LVL1::CMMRoI#CMMRoIs',
    'CaloCellContainer_HLT_TrigCaloCellMaker_eGamma',
    'CaloCellContainer_HLT_TrigCaloCellMaker_tau',
    'CaloCellContainer_HLT_TrigT2CaloTauCells',
    'CaloCellLinkContainer#HLT_TrigCaloClusterMaker_topo_Link',
    'LUCID_RawDataContainer#Lucid_RawData',
    'DataVector<LVL1::JEMRoI>#JEMRoIs',
    'DataVector<LVL1::CPMRoI>#CPMRoIs',
    'CaloTowerContainer#CombinedTower',
    'InDetBSErrContainer#SCT_ByteStreamErrs',
    'ComTime_TRT_Phase#*',
    'DataVector<LVL1::JEMHits>#JEMHits',
    'TRT_BSIdErrContainer#TRT_ByteStreamIdErrs',
    'CaloCellLinkContainer#LArClusterEM7_11Nocorr_Link',
    'DataVector<LVL1::CPMHits>#CPMHits',
    'JetCollection#HLT_TrigTauJet',
    'Muon::TgcCoinDataContainer#TrigT1CoinDataCollection',
    'Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC',
    'Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC',
    'CMMJetHitsCollection#CMMJetHits',
    'TRT_BSErrContainer#TRT_ByteStreamErrs',
    'Muon::CscStripPrepDataContainer#CSC_Measurements',
    'MuonContainer#StacoESDMuonCollection',
    'MuonContainer#CaloESDMuonCollection',
    'CaloClusterContainer#HLT_TrigCaloClusterMaker_topo',
    'MuonContainer#MuidESDMuonCollection',
    'DataVector<LVL1::CPMTower>#CPMTowersOverlap',
    'CaloCellLinkContainer#LArClusterEMSofte_Link',
    'CaloCellLinkContainer#LArClusterEMFrwd_Link',
    'CMMCPHitsCollection#CMMCPHits',
    'ROIB::RoIBResult#RoIBResult',
    'CaloCellLinkContainer#LArClusterEM_Link',
    'DataVector<LVL1::JEMEtSums>#JEMEtSums',
    'DataVector<LVL1::JetElement>#JetElementsOverlap',
    'Rec::TrackParticleContainer#Combined_TrackParticles',
    'LArDigitContainer#LArDigitContainer_Thinned',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Bjet_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Bphysics_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Electron_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_FullScan_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Muon_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Photon_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Tau_EFID',
    'TrackCollection#ConvertedMuTagTracks',
    'InDetBSErrContainer#PixelByteStreamErrs',
    'TrackCollection#ConvertedMBoyMuonSpectroOnlyTracks',
    'TrackCollection#ConvertedMBoyTracks',
    'TrackCollection#ConvertedStacoTracks',
    'TrackCollection#MuGirlRefittedTracks',
    'DataVector<LVL1::CPMTower>#CPMTowers',
    'DataVector<LVL1::CMMEtSums>#CMMEtSums',
    'TrackCollection#MuidExtrapolatedTracks',
    'DataVector<LVL1::JetElement>#JetElements',
    'TrackCollection#ConvertedMuIdCBTracks',
    'TrackCollection#Combined_Tracks',
    'TrackCollection#MooreTracks',
    'TauDetailsContainer#TauRecExtraDetailsContainer',
    'Muon::MuonPRD_Container_p2<Muon::RpcCoinData_p1>_RPC_triggerHits'
    ]
if not primEGammaDPD.WriteCaloCells() :
    excludeList += ['CaloCellContainer#AllCalo']
    pass
if not primEGammaDPD.WriteIdPrepRawData() :
    excludeList += ['InDet::PixelClusterContainer#PixelClusters',
                    'InDet::SCT_ClusterContainer#SCT_Clusters',
                    'InDet::TRT_DriftCircleContainer#TRT_DriftCircles',
                    'InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap']
    pass
if not primEGammaDPD.ApplyEGammaTrackThinning() \
       and not primEGammaDPD.ApplyMuonTrackThinning() \
       and not primEGammaDPD.ApplyTauTrackThinning() :
    excludeList += ['TrackCollection#Tracks']
    pass
if primEGammaDPD.UseAODTruth() :
    excludeList += ['McEventCollection#TruthEvent']
    EGammaStream.AddItem( ['McEventCollection#GEN_AOD'] ) #AOD MC Truth
    EGammaStream.AddItem( ['TruthParticleContainer#SpclMC'] ) #AOD MC Truth
    pass

excludeList = list(set(excludeList))

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)
#EGammaStream.AddItem( ["egammaContainer#ElectronAODCollection"] )




##====================================================================
## Finalize the thinning of the containers for this stream
##====================================================================
if primDPD.ApplyThinning() and primEGammaDPD.ApplyThinning() and not primDPD.WriteEGammaStream.isVirtual and not primDPD.isVirtual() :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    # Thin the TrackParticles
    if ( rec.readESD() and primEGammaDPD.ApplyCaloCellThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplyPixelClusterThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplySCTClusterThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplyTRTDriftCircleThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplyEGammaTrackThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplyMuonTrackThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplyTauTrackThinning() ) :
        augStream=MSMgr.GetStream( primDPD.WriteEGammaStream.StreamName )
        evtStream=augStream.GetEventStream()
        svcMgr += createThinningSvc( svcName="EGammaStreamThinning", outStreams=[evtStream] )
        

        # If thinning of PrepRawData is requested, also set up the region selector service
        if ( rec.readESD() and primEGammaDPD.ApplyPixelClusterThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplySCTClusterThinning() ) \
           or ( rec.readESD() and primEGammaDPD.ApplyTRTDriftCircleThinning() ) :
            
            from RegionSelector.RegionSelectorConf import RegSelSvc
            svcMgr += RegSelSvc("RegSelSvcInEGammaStream")
            svcMgr.RegSelSvcInEGammaStream.enableID    = True
            svcMgr.RegSelSvcInEGammaStream.enablePixel = True
            svcMgr.RegSelSvcInEGammaStream.enableSCT   = True
            svcMgr.RegSelSvcInEGammaStream.enableTRT   = True
        
            # Configure the thinning algorithm to use this RegionSelectorService
            thinSeqDESDM_EGAMMA.ThinContainersInEGammaStream.RegionSelectorTool = svcMgr.RegSelSvcInEGammaStream
            
            from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
            svcMgr += SCT_CablingSvc()
            
            IOVDbSvc = Service("IOVDbSvc")
            from IOVDbSvc.CondDB import conddb
            if not conddb.folderRequested('/SCT/DAQ/Configuration/ROD'):
                conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD")
                pass
            if not conddb.folderRequested('/SCT/DAQ/Configuration/MUR'):
                conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR")
                pass
            if not conddb.folderRequested('/SCT/DAQ/Configuration/RODMUR'):
                conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR")
                pass
            if not conddb.folderRequested('/SCT/DAQ/Configuration/Geog'):
                conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog")
                pass
            svcMgr.SCT_CablingSvc.DataSource = "CORACOOL"
            pass

        pass # End: if PRD thinning
        
    pass  # End: if primDPD.ApplyThinning()
