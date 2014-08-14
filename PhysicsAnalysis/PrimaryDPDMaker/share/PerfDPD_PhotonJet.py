##-----------------------------------------------------------------------------
## Name: PrimaryDPD_PhotonJetStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the Photon-Jet DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_PhotonJet.py")


## for messaging
from AthenaCommon.Logging import logging
photonjetDPDStream_msg = logging.getLogger( 'PrimaryDPD_DiJetStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_PhotonJetStream import primPhotonJetDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    photonjetDPDStream_msg.info('Values of all PerfDPD_PhotonJet flags:')    
    print primPhotonJetDPD
    pass





# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdPhotonJetSequence = CfgMgr.AthSequencer( 'DESD_PHOJET_Sequence',
                                             StopOverride = True )
topSequence += desdPhotonJetSequence




##============================================================================
## Define the skimming (event selection) for the DESD_PHOJET output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner

# Create the command string for the final decision
phojetCMDString = ""
algsToBookkeep = []



# Do the trigger selection
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
desdPhotonJetSequence += TriggerFilter( "PhotonJetStream_TriggerFilter1",
                              trigger = primPhotonJetDPD.TriggerFilter.Trigger1
                              )

desdPhotonJetSequence += TriggerFilter( "PhotonJetStream_TriggerFilter2",
                              trigger = primPhotonJetDPD.TriggerFilter.Trigger2
                              )

desdPhotonJetSequence += TriggerFilter( "PhotonJetStream_TriggerFilter3",
                              trigger = primPhotonJetDPD.TriggerFilter.Trigger3
                              )



if primPhotonJetDPD.LowEtPhotonFilter():
    desdPhotonJetSequence += D2PDPhotonSelector( "PhotonJetStream_LowEtPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'PhoJetStream_LowEtPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primPhotonJetDPD.LowEtPhotonFilter.PhotonEt,
                                       photonID             = primPhotonJetDPD.LowEtPhotonFilter.PhotonID
                                       )

    desdPhotonJetSequence += PrimaryDPDPrescaler( "PhotonJetStream_LowEtPhotonPrescaleFilter",
                                        AcceptAlgs  = ["PhotonJetStream_TriggerFilter1"],
                                        RequireAlgs = ["PhotonJetStream_LowEtPhotonFilter"],
                                        Prescale = primPhotonJetDPD.LowEtPhotonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("PhotonJetStream_LowEtPhotonPrescaleFilter")
    phojetCMDString += "PhotonJetStream_LowEtPhotonPrescaleFilter"
    pass


if phojetCMDString != "" and not phojetCMDString.endswith(" or "):
    phojetCMDString += " or "
    pass


if primPhotonJetDPD.MediumEtPhotonFilter():
    desdPhotonJetSequence += D2PDPhotonSelector( "PhotonJetStream_MediumEtPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'PhoJetStream_MediumEtPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primPhotonJetDPD.MediumEtPhotonFilter.PhotonEt,
                                       photonID             = primPhotonJetDPD.MediumEtPhotonFilter.PhotonID
                                       )

    desdPhotonJetSequence += PrimaryDPDPrescaler( "PhotonJetStream_MediumEtPhotonPrescaleFilter",
                                        AcceptAlgs  = ["PhotonJetStream_TriggerFilter2"],
                                        RequireAlgs = ["PhotonJetStream_MediumEtPhotonFilter"],
                                        Prescale = primPhotonJetDPD.MediumEtPhotonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("PhotonJetStream_MediumEtPhotonPrescaleFilter")
    phojetCMDString += "PhotonJetStream_MediumEtPhotonPrescaleFilter"
    pass


if phojetCMDString != "" and not phojetCMDString.endswith(" or "):
    phojetCMDString += " or "
    pass


if primPhotonJetDPD.HighEtPhotonFilter():
    desdPhotonJetSequence += D2PDPhotonSelector( "PhotonJetStream_HighEtPhotonFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'PhotonAODCollection',
                                       outputLinkCollection = 'PhoJetStream_HighEtPhotonLinkCollection',
                                       minNumberPassed      = 1,
                                       clusterEtMin         = primPhotonJetDPD.HighEtPhotonFilter.PhotonEt,
                                       photonID             = primPhotonJetDPD.HighEtPhotonFilter.PhotonID
                                       )

    desdPhotonJetSequence += PrimaryDPDPrescaler( "PhotonJetStream_HighEtPhotonPrescaleFilter",
                                        AcceptAlgs = ["PhotonJetStream_HighEtPhotonFilter"],
                                        Prescale = primPhotonJetDPD.HighEtPhotonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("PhotonJetStream_HighEtPhotonPrescaleFilter")
    phojetCMDString += "PhotonJetStream_HighEtPhotonPrescaleFilter"
    pass


if phojetCMDString != "" and not phojetCMDString.endswith(" or "):
    phojetCMDString += " or "
    pass


if primPhotonJetDPD.ZeeFilter() :
    desdPhotonJetSequence += D2PDElectronSelector( "PhotonJetStream_DoubleElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DESD_PHOJETStream_DoubleElectronLinkCollection',
                                         minNumberPassed        = 2,
                                         electronVetoAuthorList = primPhotonJetDPD.ZeeFilter.VetoElectronAuthor,
                                         electronID             = primPhotonJetDPD.ZeeFilter.ElectronID,
                                         clusterEtMin           = primPhotonJetDPD.ZeeFilter.ElectronEt
                                         )
    # Define the Z -> ee filter
    desdPhotonJetSequence += D2PDParticleCombiner( "PhotonJetStream_ZeeFilter",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'DESD_PHOJETStream_DoubleElectronLinkCollection',
                                                                     'DESD_PHOJETStream_DoubleElectronLinkCollection' ],
                                         outputCollection        = 'DESD_PHOJETStream_ZeeBosonCollection',
                                         minNumberPassed         = 1,
                                         ptMin                   = primPhotonJetDPD.ZeeFilter.MinPt,
                                         #massMin                 = primPhotonJetDPD.ZeeFilter.MaxMass,
                                         massMin                 = primPhotonJetDPD.ZeeFilter.MinMass
                                         )
    # Define the prescaled filters; configurable with command line argument
    desdPhotonJetSequence += PrimaryDPDPrescaler( "PhotonJetStream_ZeePrescaleFilter",
                                        AcceptAlgs = ["PhotonJetStream_ZeeFilter"],
                                        Prescale = primPhotonJetDPD.ZeeFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("PhotonJetStream_ZeePrescaleFilter")
    phojetCMDString += "PhotonJetStream_ZeePrescaleFilter"
    pass


# Remove the trailing "or" in the list
if phojetCMDString.endswith(" or "):
    phojetCMDString = phojetCMDString[:-len(" or ")]
    pass

# Create the combined decission
desdPhotonJetSequence += LogicalFilterCombiner( "StreamDESD_PHOJET_AcceptEvent",
                                                cmdstring = phojetCMDString
                                                )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_PHOJET_AcceptEvent" )






##====================================================================
## Define the Photon-Jet DPD output stream
##====================================================================
streamName = primDPD.WritePhotonJetStream.StreamName
fileName   = buildFileName( primDPD.WritePhotonJetStream )
if primDPD.WritePhotonJetStream.isVirtual or primDPD.isVirtual() :
    PhotonJetStream = MSMgr.NewVirtualStream(streamName, fileName)
    pass
else:
    PhotonJetStream = MSMgr.NewPoolStream(streamName, fileName)
    PhotonJetStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPD.ApplySkimming() and primPhotonJetDPD.ApplySkimming() :
    # Now, register the fitlers with the output stream
    PhotonJetStream.AcceptAlgs( [ "StreamDESD_PHOJET_AcceptEvent" ] )
    pass




#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput


# Take all items from the input, except for the ones listed in the excludeList
excludeList=[ "TrigMuonEFContainer#HLT_MuonEF" ]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)
#PhotonJetStream.AddItem( ["egammaContainer#ElectronAODCollection"] )



