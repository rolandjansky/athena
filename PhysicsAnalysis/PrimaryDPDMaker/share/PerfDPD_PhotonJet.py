##-----------------------------------------------------------------------------
## Name: PrimaryDPD_PhotonJetStream.py
##
## Author: Chris Young
## Email:  christopher.young@cern.ch
##
## Description: This defines the content of the high pt Photon DPD output stream.
##
##-----------------------------------------------------------------------------

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DESDM_PHOJET_Seq = CfgMgr.AthSequencer("DESDM_PHOJET_Seq")


##============================================================================
## Define the skimming (event selection) for the DESD_PHOJET output stream
##============================================================================
# Object selection strings
sel_phojet  = 'Photons.pt > 190*GeV && Photons.Tight'

# Event selection string
desd_phojet = '(count('+sel_phojet+')>=1)'

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DESDM_PHOJET_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_PHOJET_SkimmingTool",
                                                                        expression = desd_phojet)

ToolSvc += DESDM_PHOJET_SkimmingTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DESDM_PHOJET_Seq += CfgMgr.DerivationFramework__DerivationKernel("DESDM_PHOJETKernel",
                                                               SkimmingTools = [DESDM_PHOJET_SkimmingTool]
                                                               )
topSequence += DESDM_PHOJET_Seq


##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteDESDM_PHOJETStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_PHOJETStream )

if primDPD.WriteDESDM_PHOJETStream.isVirtual or primDPD.isVirtual() :
    StreamDESDM_PHOJET=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    StreamDESDM_PHOJET=MSMgr.NewPoolStream( streamName, fileName )
    pass

StreamDESDM_PHOJET.AddRequireAlgs(["DESDM_PHOJETKernel"])


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList, list copied from AllCells
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



