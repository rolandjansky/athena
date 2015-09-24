##-----------------------------------------------------------------------------
## Name: PrimaryDPD_TtbarMuon.py
##
## Author: Chris Young
## Email:  christopher.young@cern.ch
##
## Description: This defines the content of the ttbar single muon DPD output stream.
##
##-----------------------------------------------------------------------------

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DESDM_SLTTMU_Seq = CfgMgr.AthSequencer("DESDM_SLTTMU_Seq")


##============================================================================
## Define the skimming (event selection) for the DESD_PHOJET output stream
##============================================================================
# Object selection strings
sel_muon  = 'Muons.pt > 24*GeV && Muons.ptcone20/Muons.pt < 0.2'
sel_bjet  = 'AntiKt4EMTopoJets.pt > 25*GeV && abs(AntiKt4EMTopoJets.eta) < 2.5 && BTagging_AntiKt4EMTopo.MV2c20_discriminant>0.0314'

# Event selection string
desd_ttbar = '(count('+sel_muon+')>=1 && count('+sel_bjet+')>=1)'


# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DESDM_SLTTMU_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_SLTTMU_SkimmingTool",
                                                                        expression = desd_ttbar)

ToolSvc += DESDM_SLTTMU_SkimmingTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DESDM_SLTTMU_Seq += CfgMgr.DerivationFramework__DerivationKernel("DESDM_SLTTMUKernel",
                                                               SkimmingTools = [DESDM_SLTTMU_SkimmingTool]
                                                               )
topSequence += DESDM_SLTTMU_Seq


##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteDESDM_SLTTMUStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_SLTTMUStream )

if primDPD.WriteDESDM_SLTTMUStream.isVirtual or primDPD.isVirtual() :
    StreamDESDM_SLTTMU=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    StreamDESDM_SLTTMU=MSMgr.NewPoolStream( streamName, fileName )
    pass

StreamDESDM_SLTTMU.AddRequireAlgs(["DESDM_SLTTMUKernel"])


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



