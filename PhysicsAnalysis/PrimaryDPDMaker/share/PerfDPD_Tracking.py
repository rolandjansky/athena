##-----------------------------------------------------------------------------
## Name: PrimaryDPD_Tracking.py
##
## Author: Lorenzo Feligioni
## Email:  Lorenzo.Feligioni@cern.ch
##
## Based on:  PrimaryDPD_PhotonJetStream.py by Karsten Koeneke (DESY)
##
## Description: This defines the content of the Tracking DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_Tracking.py")


## for messaging
from AthenaCommon.Logging import logging
trackDPDStream_msg = logging.getLogger( 'PrimaryDPD_Tracking' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_Tracking import primTrackDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    trackDPDStream_msg.info('Values of all PerfDPD_Tracking flags:')
    print primTrackDPD
    pass





# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdTrackingSequence = CfgMgr.AthSequencer( 'DESDM_TRACK_Sequence',
                                            StopOverride = True )
topSequence += desdTrackingSequence



##====================================================================
## Setup TrigDecisionTool for the Jet DPD output stream
##====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")


from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler



##============================================================================
## Define the skimming (event selection) for the DESDM_TRACK output stream
##============================================================================
TrackingCmdString = ""
algsToBookkeep = []


if primTrackDPD.ApplyTriggerSkimming() :
    # Have to include this jobOption to set up the trigger info!!!
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

    # Now, define the trigger filters
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    runningIndex = 0
    TriggerNames = {}
    globalTriggerPrescale = primTrackDPD.GlobalTriggerPrescale

    if primTrackDPD.ApplyL1SingleJetTriggerSkimming() :
        for k, v in primTrackDPD.L1SingleJetTriggerNames.iteritems():
            TriggerNames[k] = v
            pass
        pass
    if primTrackDPD.ApplyL1MuJetTriggerSkimming() :
        for k, v in primTrackDPD.L1MuJetTriggerNames.iteritems():
            TriggerNames[k] = v
            pass
        pass
    if primTrackDPD.ApplyL2MuJetTriggerSkimming() :
        for k, v in primTrackDPD.L2MuJetTriggerNames.iteritems():
            TriggerNames[k] = v
            pass
        pass
    if primTrackDPD.ApplyEFMuJetTriggerSkimming() :
        for k, v in primTrackDPD.EFMuJetTriggerNames.iteritems():
            TriggerNames[k] = v
            pass
        pass
    if primTrackDPD.ApplyBJetSliceTriggerSkimming() :
        for k, v in primTrackDPD.BJetSliceTriggerNames.iteritems():
            TriggerNames[k] = v
            pass
        pass
    for trigName in TriggerNames.keys() :
        runningIndex += 1
        TriggerFilterName = "TrackingStream_TriggerFilter_"+trigName
        desdTrackingSequence += TriggerFilter( TriggerFilterName,
                                      trigger = trigName )
        if not primTrackDPD.ApplyTriggerPrescales() :
            # Add the per-event bookkeeping of this filer
            algsToBookkeep.append(TriggerFilterName)
            TrackingCmdString += TriggerFilterName
            pass
        else :
            prescale = globalTriggerPrescale*TriggerNames[trigName] if TriggerNames[trigName]>0 else 1
            TriggerPrescaleName = "TrackingStream_Prescale_"+trigName
            desdTrackingSequence += PrimaryDPDPrescaler(TriggerPrescaleName,
                                                        AcceptAlgs = [TriggerFilterName],
                                                        Prescale   =  prescale)
            # Add the per-event bookkeeping of this filer
            algsToBookkeep.append(TriggerPrescaleName)
            TrackingCmdString += TriggerPrescaleName
            pass
        if not runningIndex == TriggerNames.__len__() :
            TrackingCmdString += " or "
            pass
        pass
    pass

if primTrackDPD.ApplyJetSkimming() :

    from PrimaryDPDMaker.JetFilter import JetFilter
    desdTrackingSequence += JetFilter("TrackingStream_JetEt0to30Filter")
    desdTrackingSequence.TrackingStream_JetEt0to30Filter.cutEtMin         = 0.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt0to30Filter.cutEtMax         = 30.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt0to30Filter.cutEtaMax        = 2.5
        
    desdTrackingSequence += JetFilter("TrackingStream_JetEt30to60Filter")
    desdTrackingSequence.TrackingStream_JetEt30to60Filter.cutEtMin         = 30.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt30to60Filter.cutEtMax         = 60.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt30to60Filter.cutEtaMax        = 2.5
    
    desdTrackingSequence += JetFilter("TrackingStream_JetEt60to100Filter")
    desdTrackingSequence.TrackingStream_JetEt60to100Filter.cutEtMin         = 60.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt60to100Filter.cutEtMax         = 100.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt60to100Filter.cutEtaMax        = 2.5
    
    desdTrackingSequence += JetFilter("TrackingStream_JetEt100to150Filter")
    desdTrackingSequence.TrackingStream_JetEt100to150Filter.cutEtMin         = 100.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt100to150Filter.cutEtMax         = 150.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt100to150Filter.cutEtaMax        = 2.5
    
    desdTrackingSequence += JetFilter("TrackingStream_JetEt150to225Filter")
    desdTrackingSequence.TrackingStream_JetEt150to225Filter.cutEtMin         = 150.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt150to225Filter.cutEtMax         = 225.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt150to225Filter.cutEtaMax        = 2.5
    
    desdTrackingSequence += JetFilter("TrackingStream_JetEt225to375Filter")
    desdTrackingSequence.TrackingStream_JetEt225to375Filter.cutEtMin         = 225.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt225to375Filter.cutEtMax         = 375.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt225to375Filter.cutEtaMax        = 2.5
    
    desdTrackingSequence += JetFilter("TrackingStream_JetEt375to1E9Filter")
    desdTrackingSequence.TrackingStream_JetEt375to1E9Filter.cutEtMin         = 375.*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt375to1E9Filter.cutEtMax         = 1E9*Units.GeV
    desdTrackingSequence.TrackingStream_JetEt375to1E9Filter.cutEtaMax        = 2.5

    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt0to30Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt0to30Filter"],
                                                 Prescale   = primTrackDPD.JetEt0to30Prescale() )   
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt0to30Prescaler")
    
    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt30to60Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt30to60Filter"],
                                                 Prescale   = primTrackDPD.JetEt30to60Prescale() )   
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt30to60Prescaler")

    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt60to100Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt60to100Filter"],
                                                 Prescale   = primTrackDPD.JetEt60to100Prescale() )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt60to100Prescaler")

    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt100to150Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt100to150Filter"],
                                                 Prescale   = primTrackDPD.JetEt100to150Prescale() )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt100to150Prescaler")

    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt150to225Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt150to225Filter"],
                                                 Prescale   = primTrackDPD.JetEt150to225Prescale() )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt150to225Prescaler")

    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt225to375Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt225to375Filter"],
                                                 Prescale   = primTrackDPD.JetEt225to375Prescale() )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt225to375Prescaler")

    desdTrackingSequence += PrimaryDPDPrescaler( "TrackingStream_JetEt375to1E9Prescaler",
                                                 AcceptAlgs = ["TrackingStream_JetEt375to1E9Filter"],
                                                 Prescale   = primTrackDPD.JetEt375to1E9Prescale() )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_JetEt375to1E9Prescaler")

    if primTrackDPD.ApplyTriggerSkimming() :
        TrackingCmdString += " or "
        pass
    TrackingCmdString += "TrackingStream_JetEt0to30Prescaler or TrackingStream_JetEt30to60Prescaler or TrackingStream_JetEt60to100Prescaler or TrackingStream_JetEt100to150Prescaler or TrackingStream_JetEt150to225Prescaler or TrackingStream_JetEt225to375Prescaler or TrackingStream_JetEt375to1E9Prescaler"
    pass

if primTrackDPD.ApplyMuJetSkimming() :
    from PrimaryDPDMaker.MuJetFilter import MuJetFilter
    desdTrackingSequence += MuJetFilter("TrackingStream_MuJetFilter")
    if primTrackDPD.ApplyJetSkimming() or primTrackDPD.ApplyTriggerSkimming() :
        TrackingCmdString += " or "
        pass
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TrackingStream_MuJetFilter")
    TrackingCmdString += "TrackingStream_MuJetFilter"         
    pass

desdTrackingSequence += LogicalFilterCombiner( "StreamDESDM_TRACK_AcceptEvent",
                                               cmdstring = TrackingCmdString )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESDM_TRACK_AcceptEvent" )






##====================================================================
## Define the Track DPD output stream
##====================================================================
streamName = primDPD.WriteTrackingStream.StreamName
fileName   = buildFileName( primDPD.WriteTrackingStream )
if primDPD.WriteTrackingStream.isVirtual or primDPD.isVirtual() :
    TrackingStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    TrackingStream = MSMgr.NewPoolStream( streamName, fileName )
    TrackingStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass


if primDPD.ApplySkimming() and primTrackDPD.ApplySkimming() :
    TrackingStream.AcceptAlgs( [  "StreamDESDM_TRACK_AcceptEvent" ] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
#Tracking.Stream.TakeItemsFromInput = True
excludeList=[
#    'Rec::TrackParticleContainer#TrackParticleCandidate',
    'CaloCellContainer#AllCalo',
    'CaloTopoTowerContainer#TopoTower', # Trying to fix https://savannah.cern.ch/bugs/?67166
    'CaloClusterContainer#egClusterCollection',
    'CaloCellLinkContainer#egClusterCollection*',
    'CaloShowerContainer#egClusterCollection*',
    'CaloClusterContainer#EMTopoCluster*',
    'CaloCellLinkContainer#EMTopoCluster*',
    'CaloShowerContainer#EMTopoCluster*',
    'CaloCompactCellContainer#AODCellContainer', ## Requires detector description
    'CaloClusterContainer#LArClusterEM*',
    'CaloShowerContainer#LArClusterEM*',
    'CaloCellLinkContainer#LArClusterEM*',
    'CaloShowerContainer#egClusterCollection*',
    'TrackCollection#StandaloneTRTTracks',
    'TrackCollection#ResolvedPixelTracks'
    ]

dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

