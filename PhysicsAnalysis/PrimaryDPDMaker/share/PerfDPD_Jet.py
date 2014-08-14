##-----------------------------------------------------------------------------
## Name: PrimaryDPD_JetStream.py
##
## Author: Eric Feng (Chicago)
## Email:  Eric.Feng@cern.ch
##
## Based on:  PrimaryDPD_PhotonJetStream.py by Karsten Koeneke (DESY)
##
## Description: This defines the content of the Jet DPD output stream.
##
## $Id: PerfDPD_Jet.py,v 1.1 2009-03-31 12:09:39 kkoeneke Exp $
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_Jet.py")


## for messaging
from AthenaCommon.Logging import logging
jetDPDStream_msg = logging.getLogger( 'PrimaryDPD_JetStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_JetStream import primJetDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr # For release 14.1.0.2 and newer



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    jetDPDStream_msg.info('Values of all PerfDPD_Jet flags:')    
    print primJetDPD
    pass





# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdCalJetSequence = CfgMgr.AthSequencer( 'DESD_CALJET_Sequence',
                                          StopOverride = True )
topSequence += desdCalJetSequence



##====================================================================
## Setup TrigDecisionTool for the Jet DPD output stream
##====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler

# ============================================================================
# Check if TileDigitsFlt is available
# ============================================================================
from RecExConfig.ObjKeyStore import objKeyStore
hasTileDigitsFlt = False
if objKeyStore.isInInput("TileDigitsContainer","TileDigitsFlt") \
    or objKeyStore['transient'].has_item("TileDigitsContainer#TileDigitsFlt"):
    hasTileDigitsFlt = True

##============================================================================
## Define the skimming (event selection) for the DESD_JET output stream
##============================================================================

# Set up the command string that will contain the logical filter combination
caljetCMDString = ""
algsToBookkeep = []



if primJetDPD.JetTriggerFilter() and primJetDPD.doOfflineSelection() :
    caljetCMDString = "( ("
elif primJetDPD.JetTriggerFilter() or primJetDPD.doOfflineSelection() :
    caljetCMDString = "("
    pass


# Define the offline filters
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
if primJetDPD.doOfflineSelection() :
    desdCalJetSequence += D2PDJetSelector( "JetStream_JetEt1Filter",
                              inputCollection     = primJetDPD.JetEt1Filter.JetCollectionName,
                              jetScaledEtMin      = primJetDPD.JetEt1Filter.JetEtMin,
                              jetScaledEtMax      = primJetDPD.JetEt1Filter.JetEtMax,
                              absEtaMin           = primJetDPD.JetEt1Filter.JetEtaMin,
                              absEtaMax           = primJetDPD.JetEt1Filter.JetEtaMax,
                              minNumberPassed     = primJetDPD.JetEt1Filter.minNumberPassed )
    
    desdCalJetSequence += D2PDJetSelector( "JetStream_JetEt2Filter",
                              inputCollection     = primJetDPD.JetEt2Filter.JetCollectionName,
                              jetScaledEtMin      = primJetDPD.JetEt2Filter.JetEtMin,
                              jetScaledEtMax      = primJetDPD.JetEt2Filter.JetEtMax,
                              absEtaMin           = primJetDPD.JetEt2Filter.JetEtaMin,
                              absEtaMax           = primJetDPD.JetEt2Filter.JetEtaMax,
                              minNumberPassed     = primJetDPD.JetEt2Filter.minNumberPassed )

    desdCalJetSequence += D2PDJetSelector( "JetStream_JetEt3Filter",
                              inputCollection     = primJetDPD.JetEt3Filter.JetCollectionName,
                              jetScaledEtMin      = primJetDPD.JetEt3Filter.JetEtMin,
                              jetScaledEtMax      = primJetDPD.JetEt3Filter.JetEtMax,
                              absEtaMin           = primJetDPD.JetEt3Filter.JetEtaMin,
                              absEtaMax           = primJetDPD.JetEt3Filter.JetEtaMax,
                              minNumberPassed     = primJetDPD.JetEt3Filter.minNumberPassed )

    desdCalJetSequence += D2PDJetSelector( "JetStream_JetEt4Filter",
                              inputCollection     = primJetDPD.JetEt4Filter.JetCollectionName,
                              jetScaledEtMin      = primJetDPD.JetEt4Filter.JetEtMin,
                              jetScaledEtMax      = primJetDPD.JetEt4Filter.JetEtMax,
                              absEtaMin           = primJetDPD.JetEt4Filter.JetEtaMin,
                              absEtaMax           = primJetDPD.JetEt4Filter.JetEtaMax,
                              minNumberPassed     = primJetDPD.JetEt4Filter.minNumberPassed )
    
    desdCalJetSequence += D2PDJetSelector( "JetStream_JetEt5Filter",
                              inputCollection     = primJetDPD.JetEt5Filter.JetCollectionName,
                              jetScaledEtMin      = primJetDPD.JetEt5Filter.JetEtMin,
                              jetScaledEtMax      = primJetDPD.JetEt5Filter.JetEtMax,
                              absEtaMin           = primJetDPD.JetEt5Filter.JetEtaMin,
                              absEtaMax           = primJetDPD.JetEt5Filter.JetEtaMax,
                              minNumberPassed     = primJetDPD.JetEt5Filter.minNumberPassed )

    desdCalJetSequence += D2PDJetSelector( "JetStream_JetEt6Filter",
                              inputCollection   = primJetDPD.JetEt6Filter.JetCollectionName,
                              jetScaledEtMin      = primJetDPD.JetEt6Filter.JetEtMin,
                              jetScaledEtMax      = primJetDPD.JetEt6Filter.JetEtMax,
                              absEtaMin           = primJetDPD.JetEt6Filter.JetEtaMin,
                              absEtaMax           = primJetDPD.JetEt6Filter.JetEtaMax,
                              minNumberPassed     = primJetDPD.JetEt6Filter.minNumberPassed )
    
    if hasTileDigitsFlt:
        from TileRecAlgs.TileRecAlgsConf import TileCellSelector
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.dcs.Tile_setOn()
        
        desdCalJetSequence += TileCellSelector('JetStream_Tile1Filter')
        desdCalJetSequence.JetStream_Tile1Filter.CellContainerName       = primJetDPD.TileSel1Filter.CellContName
        desdCalJetSequence.JetStream_Tile1Filter.DigitsContainerName     = primJetDPD.TileSel1Filter.DigitsContName
        desdCalJetSequence.JetStream_Tile1Filter.RawChannelContainerName = primJetDPD.TileSel1Filter.RawChannelContName
        desdCalJetSequence.JetStream_Tile1Filter.MinEnergyCell           = primJetDPD.TileSel1Filter.TileSelMinEnCell
        desdCalJetSequence.JetStream_Tile1Filter.MaxEnergyCell           = primJetDPD.TileSel1Filter.TileSelMaxEnCell
        desdCalJetSequence.JetStream_Tile1Filter.MinEnergyChan           = primJetDPD.TileSel1Filter.TileSelMinEnChan
        desdCalJetSequence.JetStream_Tile1Filter.MaxEnergyChan           = primJetDPD.TileSel1Filter.TileSelMaxEnChan
        desdCalJetSequence.JetStream_Tile1Filter.MinEnergyGap            = primJetDPD.TileSel1Filter.TileSelMinEnGap
        desdCalJetSequence.JetStream_Tile1Filter.MaxEnergyGap            = primJetDPD.TileSel1Filter.TileSelMaxEnGap
        desdCalJetSequence.JetStream_Tile1Filter.MinEnergyMBTS           = primJetDPD.TileSel1Filter.TileSelMinEnMBTS
        desdCalJetSequence.JetStream_Tile1Filter.MaxEnergyMBTS           = primJetDPD.TileSel1Filter.TileSelMaxEnMBTS
        desdCalJetSequence.JetStream_Tile1Filter.SkipMasked              = primJetDPD.TileSel1Filter.TileSelSkipMasked
        desdCalJetSequence.JetStream_Tile1Filter.SkipMBTS                = primJetDPD.TileSel1Filter.TileSelSkipMBTS
        desdCalJetSequence.JetStream_Tile1Filter.CheckDCS                = primJetDPD.TileSel1Filter.TileSelCheckDCS
        desdCalJetSequence.JetStream_Tile1Filter.CheckJumps              = primJetDPD.TileSel1Filter.TileSelCheckJumps
        desdCalJetSequence.JetStream_Tile1Filter.CheckDMUs               = primJetDPD.TileSel1Filter.TileSelCheckDMUs
        desdCalJetSequence.JetStream_Tile1Filter.CheckOverLG             = primJetDPD.TileSel1Filter.TileSelCheckOverLG
        desdCalJetSequence.JetStream_Tile1Filter.CheckOverHG             = primJetDPD.TileSel1Filter.TileSelCheckOverHG
        #desdCalJetSequence.JetStream_Tile1Filter.OutputLevel             = 1
    
        # Define the prescaled filters; configurable with command line argument
        desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledTile1Filter",
                                                   AcceptAlgs = ["JetStream_Tile1Filter"],
                                                   Prescale = primJetDPD.TileSel1Filter.Prescale )
        # Add the per-event bookkeeping of this filer
        algsToBookkeep.append("JetStream_PrescaledTile1Filter")

    
    desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledJetEt1Filter",
                                               AcceptAlgs = ["JetStream_JetEt1Filter"],
                                               Prescale = primJetDPD.JetEt1Filter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("JetStream_PrescaledJetEt1Filter")
    
    desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledJetEt2Filter",
                                        AcceptAlgs = ["JetStream_JetEt2Filter"],
                                        Prescale = primJetDPD.JetEt2Filter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("JetStream_PrescaledJetEt2Filter")
    
    desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledJetEt3Filter",
                                        AcceptAlgs = ["JetStream_JetEt3Filter"],
                                        Prescale = primJetDPD.JetEt3Filter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("JetStream_PrescaledJetEt3Filter")
    
    desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledJetEt4Filter",
                                        AcceptAlgs = ["JetStream_JetEt4Filter"],
                                        Prescale = primJetDPD.JetEt4Filter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("JetStream_PrescaledJetEt4Filter")
    
    desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledJetEt5Filter",
                                        AcceptAlgs = ["JetStream_JetEt5Filter"],
                                        Prescale = primJetDPD.JetEt5Filter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("JetStream_PrescaledJetEt5Filter")
    
    desdCalJetSequence += PrimaryDPDPrescaler( "JetStream_PrescaledJetEt6Filter",
                                        AcceptAlgs = ["JetStream_JetEt6Filter"],
                                        Prescale = primJetDPD.JetEt6Filter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("JetStream_PrescaledJetEt6Filter")

    # Fill the command string for the filter combination
    if hasTileDigitsFlt:
        caljetCMDString += "JetStream_PrescaledJetEt1Filter or JetStream_PrescaledJetEt2Filter or JetStream_PrescaledJetEt3Filter or JetStream_PrescaledJetEt4Filter or JetStream_PrescaledJetEt5Filter or JetStream_PrescaledJetEt6Filter or JetStream_PrescaledTile1Filter"
    else:
        caljetCMDString += "JetStream_PrescaledJetEt1Filter or JetStream_PrescaledJetEt2Filter or JetStream_PrescaledJetEt3Filter or JetStream_PrescaledJetEt4Filter or JetStream_PrescaledJetEt5Filter or JetStream_PrescaledJetEt6Filter"

    pass



if primJetDPD.JetTriggerFilter() and primJetDPD.doOfflineSelection() :
    caljetCMDString += ") and ("
    pass
#else :
#    if caljetCMDString != "" and not caljetCMDString.endswith(" or "):
#        caljetCMDString += " or "
#        pass
#    pass




# Do the trigger selection that will be AND-ed with the offline selection
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
if primJetDPD.JetTriggerFilter():
    for trigNamePrescale in primJetDPD.JetTriggerFilter.TriggerNames :
        TriggerFilterName         = "JetStream_JetTriggerFilter_"+trigNamePrescale[0]
        TriggerPrescaleFilterName = "JetStream_JetTriggerPrescaleFilter_"+trigNamePrescale[0]
        desdCalJetSequence += TriggerFilter( TriggerFilterName,
                                      trigger = trigNamePrescale[0]
                                      )
        desdCalJetSequence += PrimaryDPDPrescaler( TriggerPrescaleFilterName,
                                            AcceptAlgs = [ TriggerFilterName ],
                                            Prescale   = trigNamePrescale[1]
                                            )
        # Add the per-event bookkeeping of this filer
        algsToBookkeep.append(TriggerPrescaleFilterName)
        caljetCMDString += TriggerPrescaleFilterName
        if caljetCMDString != "" and not caljetCMDString.endswith(" or "):
            caljetCMDString += " or "
            pass
        pass
    if primJetDPD.doOfflineSelection() and hasTileDigitsFlt:
        caljetCMDString += " JetStream_PrescaledTile1Filter or "
        pass
    pass


if primJetDPD.JetTriggerFilter() and primJetDPD.doOfflineSelection() :
    if caljetCMDString.endswith(" or "):
        caljetCMDString = caljetCMDString[:-len(" or ")]
        caljetCMDString += ") )"
        pass
elif primJetDPD.JetTriggerFilter() :
    if caljetCMDString.endswith(" or "):
        caljetCMDString = caljetCMDString[:-len(" or ")]
        caljetCMDString += ")"
        pass


desdCalJetSequence += LogicalFilterCombiner( "StreamDESDM_CALJET_OfflinePlusTriggerAcceptEvent",
                                             cmdstring = caljetCMDString )
# Add the per-event bookkeeping of this filer
algsToBookkeep.append("StreamDESDM_CALJET_OfflinePlusTriggerAcceptEvent")


# Set up the command string that will contain the logical filter combination
caljetCMDString2 = "StreamDESDM_CALJET_OfflinePlusTriggerAcceptEvent"



# Do the calibration trigger selection
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
if primJetDPD.JetCalibrationTriggerFilter():
    caljetCMDString2 += " or "
    for trigNamePrescale in primJetDPD.JetCalibrationTriggerFilter.TriggerNames :
        TriggerFilterName         = "JetStream_JetCalibrationTriggerFilter_"+trigNamePrescale[0]
        TriggerPrescaleFilterName = "JetStream_JetCalibrationTriggerPrescaleFilter_"+trigNamePrescale[0]
        desdCalJetSequence += TriggerFilter( TriggerFilterName,
                                      trigger = trigNamePrescale[0]
                                      )
        desdCalJetSequence += PrimaryDPDPrescaler( TriggerPrescaleFilterName,
                                            AcceptAlgs = [ TriggerFilterName ],
                                            Prescale   = trigNamePrescale[1]
                                            )
        # Add the per-event bookkeeping of this filer
        algsToBookkeep.append(TriggerPrescaleFilterName)
        caljetCMDString2 += TriggerPrescaleFilterName
        if caljetCMDString2 != "" and not caljetCMDString2.endswith(" or "):
            caljetCMDString2 += " or "
            pass
        pass
    pass





# Remove the last "or" in the command string
if caljetCMDString2.endswith(" or "):
    caljetCMDString2 = caljetCMDString2[:-len(" or ")]
    pass





# Create the combined decission
desdCalJetSequence += LogicalFilterCombiner( "StreamDESDM_CALJET_AcceptEvent",
                                             cmdstring = caljetCMDString2 )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESDM_CALJET_AcceptEvent" )





##====================================================================
## Define the Jet DPD output stream
##====================================================================
streamName = primDPD.WriteJetStream.StreamName
fileName   = buildFileName( primDPD.WriteJetStream )
if primDPD.WriteJetStream.isVirtual or primDPD.isVirtual() :
    JetStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    JetStream=MSMgr.NewPoolStream( streamName, fileName )
    # Add the per-event bookkeeping 
    JetStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPD.ApplySkimming() and primJetDPD.ApplySkimming() :
    # Schedule these prescaled algorithms as filters in OR
    JetStream.AcceptAlgs( [ "StreamDESDM_CALJET_AcceptEvent" ] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
#JetStream.Stream.TakeItemsFromInput = True
excludeList=[]
if primDPD.ApplyThinning() and primJetDPD.ApplyThinning() :
    excludeList=[
         'Analysis::TauDetailsContainer#HLT_TrigTauDetailsCalo',
         'Analysis::TauDetailsContainer#HLT_TrigTauDetailsMerged',
         'Analysis::TauDetailsContainer#HLT_TrigTauExtraDetailsMerged',
         'Analysis::TauJetContainer#HLT_TrigTauRecCalo',
         'Analysis::TauJetContainer#HLT_TrigTauRecMerged',
         'BCM_RDO_Container#BCM_CompactDOs',
         'BCM_RDO_Container#BCM_RDOs',
         'DataVector<LVL1::CMMCPHits>#CMMCPHits',
         'DataVector<LVL1::CMMEtSums>#CMMEtSums',
         'DataVector<LVL1::CMMEtSums>#CMMEtSums',
         'DataVector<LVL1::CMMJjetHits>#CMMEtSums',
         'DataVector<LVL1::CMMJEMHits>#JEMHits',
         'LVL1::CMMRoI#CMMRoIs',
         'DataVector<LVL1::CPMHits>#CPMHits',
         'DataVector<LVL1::CPMRoIs>#CPMRoIs',
         'DataVector<LVL1::CPMTowers>#CPMTowers',
         'CTP_Decision#CTP_Decision',
         'CTP_RDO#CTP_RDO',
         'CaloCellLinkContainer#CaloTopoCluster_Link',
         'CaloCellLinkContainer#EMTopoCluster430_Link',
         'CaloCellLinkContainer#HLT_TrigCaloClusterMaker_topo_Link',
         'CaloCellLinkContainer#MuonClusterCollection_Link',
         'CaloCellLinkContainer#Tau1P3PCellCluster_Link',
         'CaloCellLinkContainer#Tau1P3PCellEM012ClusterContainer_Link',
         'CaloCellLinkContainer#Tau1P3PPi0ClusterContainer_Link',
         'CaloClusterContainer#CaloTopoCluster',
         'CaloClusterContainer#EMTopoCluster430',
         'CaloClusterContainer#HLT',
         'CaloClusterContainer#HLT_TrigCaloClusterMaker_topo',
         'CaloClusterContainer#MuonClusterCollection',
         'CaloClusterContainer#Tau1P3PCellCluster',
         'CaloClusterContainer#Tau1P3PCellEM012ClusterContainer',
         'CaloClusterContainer#Tau1P3PPi0ClusterContainer',
         'CaloShowerContainer#CaloTopoCluster_Data',
         'CaloShowerContainer#EMTopoCluster430_Data',
         'CaloShowerContainer#HLT',
         'CaloShowerContainer#HLT_TrigCaloClusterMaker_topo_Data',
         'CaloShowerContainer#MuonClusterCollection_Data',
         'CaloShowerContainer#Tau1P3PCellCluster_Data',
         'CaloShowerContainer#Tau1P3PCellEM012ClusterContainer_Data',
         'CaloShowerContainer#Tau1P3PPi0ClusterContainer_Data',
         'CaloTowerContainer#HLT_TrigCaloTowerMaker',
         'ComTime#TRT_Phase',
         'CombinedMuonFeatureContainer#HLT',
         'CosmicMuonCollection#HLT_CosmicMuons',
         'CscRawDataContainer#CSCRDO',
         'CscSimDataCollection#CSC_SDO',
         'InDet::PixelClusterContainer#PixelClusters',
         'InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap',
         'InDet::SCT_ClusterContainer#SCT_Clusters',
         'InDet::TRT_DriftCircleContainer#TRT_DriftCircles',
         'DataVector<LVL1::JEMEtSums>#JEMEtSums'
         'DataVector<LVL1::JEMEHits>#JEMHits'
         'DataVector<LVL1::JEMERoIs>#JEMRoIs'
         'JEMRoICollection#JEMRoIs',
         'JetCollection#HLT',
         'JetCollection#HLT_TrigJetRec',
         'MissingETSigHypoContainer#EtMissHypoCollection',
         'MissingETSigObjContainer#EtMissObjCollection',
         'MuCTPI_RDO#MUCTPI_RDO',
         'Muon::ChamberT0s#MboyMuonChamberT0s',
         'Muon::ChamberT0s#MooreMuonChamberT0s',
         'Muon::CscPrepDataContainer#CSC_Clusters',
         'Muon::CscStripPrepDataContainer#CSC_Measurements',
         'Muon::MdtPrepDataContainer#MDT_DriftCircles',
         'Muon::RpcCoinDataContainer#RPC_triggerHits',
         'Muon::RpcPrepDataContainer#RPC_Measurements',
         'Muon::TgcCoinDataContainer#TrigT1CoinDataCollection',
         'Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC',
         'Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC',
         'Muon::TgcPrepDataContainer#TGC_Measurements',
         'Muon::TgcPrepDataContainer#TGC_MeasurementsNextBC',
         'Muon::TgcPrepDataContainer#TGC_MeasurementsPriorBC',
         'MuonCaloEnergyContainer#MuonCaloEnergyCollection',
         'Analysis::MuonContainer#CaloESDMuonCollection',
         'Analysis::MuonContainer#MuidESDMuonCollection',
         'Analysis::MuonContainer#StacoESDMuonCollection',
         'MuonFeatureContainer#HLT',
         'MuonSimDataCollection#MDT_SDO',
         'MuonSimDataCollection#RPC_SDO',
         'MuonSimDataCollection#TGC_SDO',
         'MuonSimDataCollection#MDT_SDO',
         'MuonSimDataCollection#RPC_SDO',
         'MuonSimDataCollection#TGC_SDO',
         'PRD_MultiTruthCollection#PRD_MultiTruthPixel',
         'PRD_MultiTruthCollection#PRD_MultiTruthSCT',
         'PRD_MultiTruthCollection#PRD_MultiTruthTRT',
         'PixelRDO_Container#PixelRDOs',
         'Rec::TrackParticleContainer#Combined_TrackParticles',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bjet_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bphysics_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_CosmicsN_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Muon_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID',
         'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_EFID',
         'Rec::TrackParticleContainer#MooreTrackParticles',
         'Rec::TrackParticleContainer#MuGirlRefittedTrackParticles',
         'Rec::TrackParticleContainer#MuTagIMOTrackParticles',
         'Rec::TrackParticleContainer#MuTagTrackParticles',
         'Rec::TrackParticleContainer#MuidCombTrackParticles',
         'Rec::TrackParticleContainer#MuidExtrTrackParticles',
         'Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles',
         'Rec::TrackParticleContainer#MuonboyTrackParticles',
         'Rec::TrackParticleContainer#StacoTrackParticles',
         'ROIB::RoIBResult#RoIBResult',
         'RpcPadContainer#RPCPAD',
         'SCT_ClusterContainer#SCT_Clusters',
         'SCT_RDO_Container#SCT_RDOs',
         'TPCnv::MuonMeasurements',
         'TRT_RDO_Container#TRT_RDOs',
         'TauDetailsContainer#HLT_TrigTauDetailsCalo',
         'Analysis::TauDetailsContainer#TauRecExtraDetailsContainer',
         'TauJetContainer#HLT_TrigTauRecCalo',
         'TileContainer<TileMu>#TileMuObj',
         'TileMuContainer#TileMuObj',
         'TileMuFeatureContainer#HLT',
         'TileTrackMuFeatureContainer#HLT',
         'TrackCollection#CombinedInDetTracks',
         'TrackCollection#Combined_Tracks',
         'TrackCollection#ConvertedMBoyMuonSpectroOnlyTracks',
         'TrackCollection#ConvertedMBoyTracks',
         'TrackCollection#ConvertedMuIdCBTracks',
         'TrackCollection#ConvertedMuTagTracks',
         'TrackCollection#ConvertedStacoTracks',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_Bjet_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_Bphysics_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_Electron_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_FullScan_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_Muon_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_Photon_EFID',
         'TrackCollection#HLT_InDetTrigTrackSlimmer_Tau_EFID',
         'TrackCollection#MooreTracks',
         'TrackCollection#MuGirlRefittedTracks',
         'TrackCollection#MuTagIMOTracks',
         'TrackCollection#MuidExtrapolatedTracks',
         'TrackCollection#MuonSlimmedTrackCollection',
         'TrackCollection#ResolvedPixelTracks',
         'TrackCollection#ResolvedSCTTracks',
         'TrackCollection#StandaloneTRTTracks',
         'TrackCollection#Tracks',
         'TrackRecordCollection#MuonEntryLayerFilter',
         'TrigEFBjetContainer#HLT_EFBjetFex',
         'TrigEFBphysContainer#HLT_EFBMuMuFex',
         'TrigEFBphysContainer#HLT_EFDsPhiPiFex',
         'TrigEFBphysContainer#HLT_EFMuPairs',
         'TrigL2BjetContainer#HLT_L2BjetFex',
         'TrigL2BphysContainer#HLT',
         'TrigL2BphysContainer#HLT_EFBMuMuFex',
         'TrigL2BphysContainer#HLT_EFDsPhiPiFex',
         'TrigL2BphysContainer#HLT_EFMuPairs',
         'TrigL2BphysContainer#HLT_L2BMuMuFex',
         'TrigL2BphysContainer#HLT_L2BMuMuXFex',
         'TrigL2BphysContainer#HLT_L2DiMuXFex',
         'TrigL2BphysContainer#HLT_L2DsPhiPiFexDs',
         'TrigL2BphysContainer#HLT_L2DsPhiPiFexPhi',
         'TrigL2BphysContainer#HLT_L2JpsieeFex',
         'TrigL2BphysContainer#HLT_L2TrackMass',
         'TrigL2BphysContainer#HLT_TrigDiMuon',
         'TrigMissingETContainer#HLT_T2MissingET',
         'TrigMissingETContainer#HLT_TrigEFMissingET',
         'TrigOperationalInfoCollection#HLT_OPI_EF',
         'TrigOperationalInfoCollection#HLT_OPI_L2',
         'TrigRoiDescriptorCollection#HLT_initialRoI',
         'TrigRoiDescriptorCollection#HLT_secondaryRoI_EF',
         'TrigRoiDescriptorCollection#HLT_secondaryRoI_L2',
         'TrigSpacePointCountsCollection#HLT_spacepoints',
         'TrigT2JetContainer#HLT_TrigT2CaloJet',
         'TrigT2MbtsBitsContainer#HLT_T2Mbts',
         'TrigTauTracksInfoCollection#HLT',
         'TrigTrackCountsCollection#HLT',
         'TrigTrackCountsCollection#HLT_trackcounts',
         'TrigVertexCollection#HLT_T2HistoPrmVtx',
         'TriggerTowerCollection#TriggerTowers',
         'PRD_MultiTruthCollection#CSC_TruthMap',
         'PRD_MultiTruthCollection#MDT_TruthMap',
         'PRD_MultiTruthCollection#RPC_TruthMap',
         'PRD_MultiTruthCollection#TGC_TruthMap',
         'Trk::SegmentCollection#ConvertedMBoySegments',
         'Trk::SegmentCollection#MooreSegments',
         'Trk::SegmentCollection#MuGirlSegments',
         'Trk::TrackCollection#Tracks',
         'VxContainer#HLT_PrimVx'
        ]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)


