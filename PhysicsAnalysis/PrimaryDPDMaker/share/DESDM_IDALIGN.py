##-----------------------------------------------------------------------------
## Name: DESDM_IDALIGN.py
##
## Author: Paul Laycock
## Email:  paul.james.laycock@cern.ch
##
## Description: Based on Allcells from Jamie, and then removing biggest Calo info for ID !
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/DESDM_IDALIGN.py")


## for messaging
from AthenaCommon.Logging import logging
allcellsDPDStream_msg = logging.getLogger( 'PrimaryDPD_PerfIDALIGNStream' )

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
#    allcellsDPDStream_msg.info('Values of all PrimaryDPDFlags_IDALIGNStream flags:')    
#    print primIDALIGNDPD
#    pass


# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdIDALIGNSequence = CfgMgr.AthSequencer( 'DESDM_IDALIGN_Sequence',
                                                  StopOverride = True )
topSequence += desdIDALIGNSequence

##====================================================================
## Define the Single Electron DPD output stream
##====================================================================
streamName = primDPD.WriteIDALIGNStream.StreamName
fileName   = buildFileName( primDPD.WriteIDALIGNStream )
if primDPD.WriteIDALIGNStream.isVirtual or primDPD.isVirtual() :
    IDALIGNStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    IDALIGNStream=MSMgr.NewPoolStream( streamName, fileName )
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
              "CaloCellContainer#AllCalo",
              ]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)


