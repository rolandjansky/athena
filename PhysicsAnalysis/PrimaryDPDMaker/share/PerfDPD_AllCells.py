##-----------------------------------------------------------------------------
## Name: PrimaryDPD_MissingEtStream.py
##
## Author: Jamie Boyd
## Email:  jamie.boyd@cern.ch
##
## Description: DESDM for jet studies where all cells are available
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_AllCells.py")


## for messaging
from AthenaCommon.Logging import logging
allcellsDPDStream_msg = logging.getLogger( 'PrimaryDPD_PerfAllcellsStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

##====================================================================
## Write the used options to the log file
##====================================================================
#if rec.OutputLevel() <= INFO:
#    allcellsDPDStream_msg.info('Values of all PrimaryDPDFlags_AllcellsStream flags:')    
#    print primAllcellsDPD
#    pass


# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdAllcellsSequence = CfgMgr.AthSequencer( 'DESDM_ALLCELLS_Sequence',
                                                  StopOverride = True )
topSequence += desdAllcellsSequence

##====================================================================
## Define the Single Electron DPD output stream
##====================================================================
streamName = primDPD.WriteAllcellsStream.StreamName
fileName   = buildFileName( primDPD.WriteAllcellsStream )
if primDPD.WriteAllcellsStream.isVirtual or primDPD.isVirtual() :
    AllcellsStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    AllcellsStream=MSMgr.NewPoolStream( streamName, fileName )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[ "TrackCollection#Tracks",
              "InDet::PixelClusterContainer#PixelClusters",
              "InDet::SCT_ClusterContainer#SCT_Clusters",
              "InDet::TRT_DriftCircleContainer#TRT_DriftCircles",
              "PRD_MultiTruthCollection#PRD_MultiTruthTRT",
              "PRD_MultiTruthCollection#CSC_TruthMap",
              "PRD_MultiTruthCollection#MDT_TruthMap",
              "PRD_MultiTruthCollection#PRD_MultiTruthPixel",
              "PRD_MultiTruthCollection#PRD_MultiTruthSCT",
              "PRD_MultiTruthCollection#RPC_TruthMap",
              "PRD_MultiTruthCollection#TGC_TruthMap",
              "Muon::CscStripPrepDataContainer#CSC_Measurements",
              "Muon::RpcPrepDataContainer#RPC_Measurements",
              "Muon::TgcPrepDataContainer#TGC_Measurements",
              "Muon::TgcPrepDataContainer#TGC_MeasurementsNextBC",
              "Muon::TgcPrepDataContainer#TGC_MeasurementsPriorBC",
              "Muon::MdtPrepDataContainer#MDT_DriftCircles",
              "Muon::RpcCoinDataContainer#RPC_triggerHits",
              "MuonSimDataCollection#MDT_SDO",
              "MuonSimDataCollection#RPC_SDO",
              "MuonSimDataCollection#TGC_SDO",
              "CscSimDataCollection#CSC_SDO",
              "CscRawDataContainer#CSCRDO",
              "Muon::CscPrepDataContainer#CSC_Clusters",
              "InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap",
              "InDet::PixelGangedClusterAmbiguities#SplitClusterAmbiguityMap",
              "MdtCsmContainer#MDTCSM",
              # not sure if the detailed track truth will be useful?
              "DetailedTrackTruthCollection#ConvertedMBoyMuonSpectroOnlyTracksTruth",
              "DetailedTrackTruthCollection#ConvertedMBoyTracksTruth",
              "DetailedTrackTruthCollection#DetailedTrackTruth",
              "DetailedTrackTruthCollection#MooreTracksTruth",
              "DetailedTrackTruthCollection#MuonSpectrometerTracksTruth",
              "DetailedTrackTruthCollection#ResolvedForwardTracksDetailedTruth",
              ]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)


